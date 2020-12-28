#!/usr/bin/env python

import os
import sys
import subprocess
import re
import unicodedata

FAUST = os.getenv("FAUST") or "faust"

PROLOGUE = '''#pragma once
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
'''

EPILOGUE = '''
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
'''

def main(args):
    process = subprocess.run([FAUST] + args[1:], capture_output=True)
    process.check_returncode()
    text = process.stdout.decode('utf-8')

    params = find_parameters(text)

    text = remove_virtual(text)
    for method in ('metadata', 'getInputRate', 'getOutputRate', 'clone', 'buildUserInterface'):
        text = remove_method(text, method)

    for method in ('getNumInputs', 'getNumOutputs'):
        text = make_method_static_constexpr(text, method)

    text = remove_superclass(text)

    text = add_parameters(text, params)

    for method in ('compute', 'classInit', 'instanceConstants', 'instanceResetUserInterface', 'instanceClear', 'init', 'instanceInit'):
        text = add_method_start_end(text, method)
    text = add_class_start_end(text)

    text = PROLOGUE + text + EPILOGUE

    sys.stdout.write(text)

def find_parameters(text):
    params = []
    lines = text.split('\n')
    expr = re.compile(r'->add(Button|CheckButton|VerticalSlider|HorizontalSlider|NumEntry|HorizontalBargraph|VerticalBargraph)\("([^"]*)", &([a-zA-Z0-9_]+)')
    for line in lines:
        match = expr.search(line)
        if match:
            params += [{
                'name': match.group(2),
                'var': match.group(3),
                'readonly': match.group(1) in ('HorizontalBargraph', 'VerticalBargraph')
            }]
    return params

def remove_virtual(text):
    return re.sub(r'\bvirtual\s*\b', '', text)

def remove_superclass(text):
    lines = text.split('\n')
    expr = re.compile(r': public (one_sample_)?dsp')

    for i in range(len(lines)):
        line = lines[i]
        new_line = re.sub(expr, '', line)
        if new_line != line:
            lines[i] = new_line
            break

    return '\n'.join(lines)

def remove_method(text, method):
    lines = text.split('\n')
    new_lines = []
    in_method = False
    eom = None
    for i in range(len(lines)):
        line = lines[i]
        stripped = line.strip()
        if in_method:
            if line.rstrip() == eom:
                in_method = False
        else:
            if method + '(' in stripped and stripped.endswith('{'):
                in_method = True
                eom = ''
                for ch in line:
                    if ch in ('\t', ' '):
                        eom += ch
                    else:
                        break
                eom += '}'
            else:
                new_lines.append(line)
    return '\n'.join(new_lines)

def make_method_static_constexpr(text, method):
    lines = text.split('\n')
    new_lines = []
    for i in range(len(lines)):
        line = lines[i]
        stripped = line.strip()
        if method + '(' in stripped and stripped.endswith('{'):
            new_lines.append(re.sub(r'^([ \t]+)', r'\1static constexpr ', line))
        else:
            new_lines.append(line)
    return '\n'.join(new_lines)

def add_method_start_end(text, method):
    lines = text.split('\n')
    new_lines = []
    in_method = False
    eom = None
    for i in range(len(lines)):
        line = lines[i]
        stripped = line.strip()
        if in_method:
            if line.rstrip() == eom:
                new_lines.append('\t\t// END %s //' % (method))
                in_method = False
            new_lines.append(line)
        else:
            new_lines.append(line)
            if method + '(' in stripped and stripped.endswith('{'):
                in_method = True
                new_lines.append('\t\t// BEGIN %s //' % (method))
                eom = ''
                for ch in line:
                    if ch in ('\t', ' '):
                        eom += ch
                    else:
                        break
                eom += '}'
    return '\n'.join(new_lines)

def add_class_start_end(text):
    *_, match = re.finditer(r'^class\b.*', text, re.MULTILINE)
    index = match.end(0)
    text = text[:index] + '\n\t// BEGIN class //\n' + text[index:]
    index = match.start(0)
    text = text[:index] + '\n// BEFORE class //\n' + text[index:]

    *_, match = re.finditer(r'^\};', text, re.MULTILINE)
    index = match.end(0)
    text = text[:index] + '\n// AFTER class //\n' + text[index:]
    index = match.start(0)
    text = text[:index] + '\n\t// END class //\n' + text[index:]

    return text

def add_to_class(text, members):
    if len(members) == 0:
        return text
    *_, match = re.finditer(r'^\};', text, re.MULTILINE)
    assert(match)
    index = match.start(0)
    return text[:index] + members + text[index:]

def add_parameters(text, params):
    members = ''
    for param in params:
        members += '\n'
        members += '	FAUSTFLOAT get%s() const { return %s; }\n' % \
            (camelify(param['name']), param['var'])
        if not param['readonly']:
            members += '	void set%s(FAUSTFLOAT value) { %s = value; }\n' % \
                (camelify(param['name']), param['var'])
    return add_to_class(text, members)

def camelify(name):
    return ''.join([simplestr(capitalize1stChar(x)) for x in name.split(' ')])

def capitalize1stChar(word):
    if len(word) < 1:
        return word
    return word[0].upper() + word[1:]

def simplechar(ch):
    dec = unicodedata.decomposition(ch)
    if len(dec) > 0:
        return chr(int(dec.split(' ')[0], 16))
    else:
        return ch

def simplestr(text):
    return ''.join(map(simplechar, text))

if __name__ == '__main__':
    main(sys.argv)
