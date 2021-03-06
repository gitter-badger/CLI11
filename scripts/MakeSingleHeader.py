#!/usr/bin/env python

# Requires pathlib on python 2

from __future__ import print_function, unicode_literals

import re
import argparse
from pathlib import Path
from subprocess import check_output

includes_local = re.compile(r"""^#include "(.*)"$""", re.MULTILINE)
includes_system = re.compile(r"""^#include \<(.*)\>$""", re.MULTILINE)

DIR = Path(__file__).resolve().parent
BDIR = DIR.parent / 'include'

TAG = check_output(['git', 'describe', '--tags', '--always'], cwd=str(DIR)).decode("utf-8")

def MakeHeader(out):
    main_header = BDIR / 'CLI/CLI.hpp'
    with main_header.open() as f:
        header = f.read()

    include_files = includes_local.findall(header)

    headers = set()
    output = ''
    for inc in include_files:
        with (BDIR / inc).open() as f:
            inner = f.read()
        headers |= set(includes_system.findall(inner))
        output += '\n// From {inc}\n\n'.format(inc=inc)
        output += inner[inner.find('namespace'):]

    header_list = '\n'.join('#include <'+h+'>' for h in headers)

    output = '''\
#pragma once

// Distributed under the MIT license.  See accompanying
// file LICENSE or https://github.com/henryiii/CLI11 for details.

// This file was generated using MakeSingleHeader.py in CLI11/scripts
// from: {tag}
// This has the complete CLI library in one file.

{header_list}
{output}'''.format(header_list=header_list, output=output, tag=TAG)

    with Path(out).open('w') as f:
        f.write(output)

    print("Created {out}".format(out=out))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("output", nargs='?', default=BDIR / 'CLI11.hpp')
    args = parser.parse_args()
    MakeHeader(args.output)
