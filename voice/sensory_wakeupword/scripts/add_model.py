"""
@file add_model.py
@brief Script for adding new Sensory models to the sensory_wakeupword demo app
@usage python scripts/add_model.py --help

*******************************************************************************
# License
<b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
The licensor of this software is Silicon Laboratories Inc. Your use of this
software is governed by the terms of Silicon Labs Master Software License
Agreement (MSLA) available at
www.silabs.com/about-us/legal/master-software-license-agreement. This
software is distributed to you in Source Code format and is governed by the
sections of the MSLA applicable to Source Code.
******************************************************************************
"""

import sys
from argparse import ArgumentParser
from dataclasses import dataclass
from io import StringIO, TextIOBase, TextIOWrapper
from pathlib import Path
from typing import Iterable, Tuple, Union


@dataclass
class Config:
    """Struct indicating the input model and output name to assign to it."""
    model: Path
    output: str = ""
    force: bool = False

    def __post_init__(self):
        self.model = self.model.expanduser().resolve()
        if not self.model.is_dir():
            raise ValueError(f"Expected `model` to be a directory but received {self.model}")
        if self.output == "":
            self.output = self.model.stem

def create_parser() -> ArgumentParser:
    parser = ArgumentParser('add_model.py', description='Add a new Sensory model to `app/include/model`')
    parser.add_argument('model', type=Path, help='Folder containing the Sensory model to add to this application')
    parser.add_argument('--output', '-o', type=str, help='The name to assign to the output model. If omitted, this uses the folder name of `model`')
    parser.add_argument('--force', '-f', action='store_true', help='Flag to allow overwriting an existing model ')
    return parser

def append(from_file: TextIOBase, to_file: TextIOWrapper):
    to_file.write(from_file.read())

def merge_file_contents_into(source_files: Iterable[Union[Path, TextIOBase]], output_file: Path):
    """Erases the existing contents of `output_file` then iteratively merges the contents in `source_files` into `output_file`."""
    with open(output_file, 'w') as outfile:
        for input_file in source_files:
            if isinstance(input_file, TextIOBase):
                append(input_file, outfile)
            else:
                with open(input_file, 'r') as infile:
                    append(infile, outfile)


def sensory_search_files(model_dir: Path) -> Tuple[Path, Path]:
    """Locates the Sensory search (grammar) files for the given model."""
    try:
        search_source_file = next(model_dir.glob('*search.c'))
        search_header_file = next(model_dir.glob('*search.h'))
    except StopIteration:
        raise ValueError(f"Expected *search.c and *search.h files in model directory {model_dir}")

    return search_header_file, search_source_file

def sensory_net_file(model_dir: Path) -> Path:
    """Locates the Sensory net files for the given model."""

    try:
        return next(model_dir.glob('*net.c'))
    except StopIteration:
        raise ValueError(f"Expected *net.c file in model directory {model_dir}")

def add_aligned_attribute(file: Path) -> int:
    count_changed_lines: int = 0
    with open(file, 'r') as f:
        lines = f.readlines()
    for i in range(len(lines)):
        line = lines[i]
        if 'const unsigned short' in line and not 'const unsigned short __ALIGNED' in line:
            lines[i] = line.replace('const unsigned short', 'const unsigned short __ALIGNED(4)')
            count_changed_lines += 1
    with open(file, 'w') as f:
        f.writelines(lines)
    return count_changed_lines

def add_model(config: Config) -> Path:
    """Adds a new model to the `sensory_wakeupword` demo application.

    This writes the model source/header files from `config.model` into
    `net.h` and `search.h` in `app/include/model/<config.output>`.
    The output is written as header files only, as these are in the existing include path
    which means no SLCP changes or project regeneration is needed afterwards.

    A #MODEL_NAME define is written to the `search.h` file so this can be displayed in the app banner.

    Returns:
        Path: The output directory the model was written to
    """
    models_directory = Path(__file__).parent.parent / "app/include/model"
    if not models_directory.exists():
        raise IOError(f"Output directory {models_directory} does not exist")
    output_directory = models_directory / config.output
    input_directory = config.model
    if output_directory.exists() and not config.force:
        raise IOError(f"Output directory {models_directory} exists. Use the `-f` flag if you want to overwrite it.")
    output_directory.mkdir(exist_ok=True)


    search_files = sensory_search_files(input_directory)
    merge_file_contents_into(
        [
            search_files[0],
            StringIO(
                f'\n\n/* The following identifier was automatically generated by {Path(__file__).name} */'
                f'\n#define MODEL_NAME "{config.output}"\n\n'
            ),
            search_files[1],
        ],
        output_directory / "search.h"
    )

    merge_file_contents_into(
        [sensory_net_file(input_directory)],
        output_directory / "net.h"
    )

    add_aligned_attribute(output_directory / "net.h")
    add_aligned_attribute(output_directory / "search.h")

    return output_directory

def main():
    parser = create_parser()
    args = parser.parse_args()
    try:
        config = Config(**vars(args))
        output_directory = add_model(config)
        print(f"Wrote {output_directory.relative_to(Path('.').resolve())}")
    except ValueError as e:
        print(e, file=sys.stderr)
        sys.exit(1)
    except IOError as e:
        print(e, file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
