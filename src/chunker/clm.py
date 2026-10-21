"""Constrained LLM decoding for function calling and parameter extraction.

This module provides the core ConstrainedLLM class and utility functions
to execute token level logit masking, ensuring that generated model outputs
strictly comply with predefined function schemas and parameter types.
"""

from typing import Any
import numpy as np
from llm_sdk import Small_LLM_Model  # type: ignore
from .data_schema import FunctionStruct, ResultDictSchema

SYSTEM_PROMPT = """
You are a presice function choicer.

your sole task is to choose the most appropriate function name

from the "Available functions".

User prompt:
{PROMPT}

Available functions:
{FUNCTIONS}

The answer is:
"""


def dictify(
        prompt: str, name: str, parameters: dict[str, Any],
) -> ResultDictSchema:
    """
    creatin a result dictionary with:
    [prompt, function name, parameters]

    Args:
        prompt: The user prompt.
        name: The function name.
        parameters: The function parameters.

    Returns:
        ResultDictSchema containing the prompt, name, and parameters.
    """
    return {
        "prompt": prompt,
        "name": name,
        "parameters": parameters
    }


class ConstrainedLLM:
    """
        generating the LLM responses
        using constrained decoding for function calling.
    """

    def __init__(
        self,
        result: list[ResultDictSchema],
        functions_definition: list[FunctionStruct],
    ) -> None:
        """
            sets up storage containers,
            schema definitions,
            and model interface.
        """
        self.llm = Small_LLM_Model()
        self.result = result
        self.functions_definition = functions_definition
        self.funcs_names_ids: list[list[int]] = []

    def _constrainig_functions(
        self, logits: list[float], index: int,
    ) -> list[float]:
        """
        mask raw model logits to restrict
        generation to valid function names.

        Args:
        logits (list[float]): unnormalized log probabilities from the LLM.
        index (int): Current sequence step index being evaluated.

        Returns:
            list[float]:
                modified logits with invalid tokens suppressed to -inf.
        """
        masked_logits = np.full_like(logits, -np.inf)

        for token_path in self.funcs_names_ids:
            if index < len(token_path):
                token = token_path[index]
                masked_logits[token] = logits[token]

        selected_token = np.argmax(masked_logits)

        self.funcs_names_ids = [
            token_path for token_path in self.funcs_names_ids
            if index >= len(token_path) or token_path[index] == selected_token
        ]

        return list(masked_logits)

    def _constrainig_parameters(
            self, logits: list[float], parameter_type: str
    ) -> list[float]:
        """
        Filters raw prediction scores according
        to the expected parameter,
        masking out non-compliant
        tokens with negative infinity.

        Args:
            logits (list[float]): unnormalized log
            probabilities from the LLM.

            parameter_type (str): Target data type
            ("number", "integer", "boolean").

        Returns:
            list[float]:
            Modified logits suppressing
            tokens that violate the schema.
        """
        masked_logits = np.full_like(logits, -np.inf)

        type_map: dict[str, Any] = {
            "number": lambda: self.llm.encode(".0123456789").tolist()[0] + [1],
            "integer": lambda: self.llm.encode("0123456789").tolist()[0] + [1],
            "boolean": lambda: [1866, 3849],
        }

        if parameter_type not in type_map:
            return logits

        allowed_tokens = type_map[parameter_type]()
        for token_id in allowed_tokens:
            masked_logits[token_id] = logits[token_id]

        return list(masked_logits)

    def _formatting_available_functions(self) -> str:
        """
        assemble all function info into one clean text list for the prompt.

        Returns:
            Formatted string of function definitions.
        """
        functions_prompt = ""
        for function_definition in self.functions_definition:
            functions_prompt += "".join(str(function_definition))
        return functions_prompt

    def _is_incomplete(self, text: str) -> bool:
        """determine whether the generated function name prefix is incomplete.

        Args:
            text (str): The current generated function name prefix.

        Returns:
            bool:
                True if the text does not yet match
                any complete function name,
                False otherwise.
        """
        names = [function.name for function in self.functions_definition]
        return text not in names

    def output_generating(self, prompt: str) -> None:
        """
        executes constrained generation loops
        to identify the function and extract parameters.

        Args:
            prompt: The user prompt to generate response for.
        """
        function_name = ""
        self.funcs_names_ids = [
            self.llm.encode(fn.name).tolist()[0]
            for fn in self.functions_definition
        ]
        prompt_text = SYSTEM_PROMPT.format(
            PROMPT=prompt, FUNCTIONS=self._formatting_available_functions()
        )

        input_ids: list[int] = self.llm.encode(prompt_text).tolist()[0]
        index = 0
        while self._is_incomplete(function_name):
            logits = self.llm.get_logits_from_input_ids(input_ids)
            logits = self._constrainig_functions(logits, index)
            next_token = int(np.argmax(logits))
            function_name += self.llm.decode(next_token)
            input_ids.append(next_token)
            index += 1

        target_function = next(
            fn for fn in self.functions_definition
            if fn.name == function_name
        )

        context_text = f"""
        <think>

        </think>

        Given the user prompt request, provide the correct parameters\
        for the function below

        Function:
        {target_function}

        User prompt:
        {prompt}

        Answer is:"""
        extracted_parameters: dict[str, Any] = {}

        for param_key, param_spec in target_function.parameters.items():
            context_text += f" \"{param_key}\": \""
            input_ids = self.llm.encode(context_text).tolist()[0]
            param_value = ""
            token_count = 0
            max_tokens = len(input_ids)
            while token_count < max_tokens:
                logits = self.llm.get_logits_from_input_ids(input_ids)
                logits = self._constrainig_parameters(
                    logits, param_spec["type"])
                next_token = int(np.argmax(logits))
                decoded_chunk = self.llm.decode(next_token)
                if '"' in decoded_chunk:
                    break
                param_value += self.llm.decode(next_token)
                input_ids.append(next_token)
                token_count += 1

            context_text += param_value
            p_type = param_spec["type"]

            if p_type == "number":
                extracted_parameters[param_key] = float(param_value)
            elif p_type == "integer":
                extracted_parameters[param_key] = int(param_value)
            elif p_type == "boolean":
                extracted_parameters[param_key] = bool(param_value)
            else:
                extracted_parameters[param_key] = str(param_value)

        self.result.append(
            dictify(prompt, function_name, extracted_parameters))
