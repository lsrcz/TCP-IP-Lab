#!/usr/bin/python3

def formatmacro(macro):
    s = 0
    ret = []
    for line in macro:
        if line.strip().endswith('}') or line.strip().endswith('})'):
            s -= 1
        toappend = '    ' * s + line
        toappend += max(79 - len(toappend), 2) * ' ' + '\\'
        ret.append(toappend)
        if line.strip().endswith('{'):
            s += 1
    ret[-1] = ret[-1][:-1].rstrip()
    return ret

def process_macro(macro):
    lfirst = macro[0]
    b = None
    e = None
    for i in range(len(lfirst)):
        if b is None and lfirst[i] == '(':
            b = i 
        if b is not None and e is None and lfirst[i] == ')':
            e = i
            break
    name = lfirst[7:b].strip()
    params = list(filter(lambda x: x != '', map(lambda x:x.split(' ')[-1].strip('*'), lfirst[b + 1:e].split(','))))
    if len(params) > 0:
        if params[-1] == '...':
            return formatmacro(macro)
        macro1 = ['#define ' + name + '(' + ', '.join(params) + ', ...) ({',
                  'ErrorBehavior _ebo = ErrorBehavior("");',
                  '_' + name + '(' + ', '.join(params) +  ', ##__VA_ARGS__, _ebo, exit(0));',
                  '})']
        macro2 = ['#define _' + name + '(' + ', '.join(params) + ', _behavior, _action, ...)',
                  '({',
                  'ErrorBehavior _eb = _behavior;',
                  '_eb.sys_msg = "' + name + '";']
    else:
        macro1 = ['#define ' + name + '(...) ({',
                'ErrorBehavior _ebo = ErrorBehavior("");',
                  '_' + name + '(##__VA_ARGS__, _ebo, exit(0));',
                  '})']
        macro2 = ['#define _' + name + '(_behavior, _action, ...)',
                  '({',
                  'ErrorBehavior _eb = _behavior;'
                  '_eb.sys_msg = "' + name + '";']
    for line in macro[1:]:
        if line.startswith('ERROR_SYSTEM'):
            macro2.insert(3, '_eb.from = SOURCE_SYSTEM;')
            macro2.append('ERROR_WITH_BEHAVIOR(_eb, _action);');
        else:
            macro2.append(line)
            
    return formatmacro(macro1) + formatmacro(macro2)

def read_next(text, pos):
    l = text[pos]
    if l.strip() == '' or l.strip().startswith('#include'):
        return (pos + 1, [l.strip()])
    if l.strip().startswith('/*'):
        ret = []
        while pos < len(text) and not text[pos].strip().startswith('*/'):
            ret.append(text[pos].rstrip())
            pos += 1
        ret.append(text[pos].rstrip())
        return (pos + 1, ret)
    if l.strip().startswith('#define'):
        ret = []
        while pos < len(text) and text[pos].strip().endswith('\\'):
            ret.append(text[pos].strip()[:-1])
            pos += 1
        ret.append(text[pos].strip())
        return (pos + 1, ret)
    return (pos + 1, l.rstrip())
    

def main():
#    name = input()
    name = './include/wrapper/unix.bak.h'
    with open(name) as f:
        code = list(f)
    pos = 0
    last_is_include = False
    while pos < len(code):
        pos, ncode = read_next(code, pos)
        if ncode[0].startswith('#define'):
            last_is_include = False
            print()
            for l in process_macro(ncode):
                print(l)
        elif ncode[0] == '':
            pass
        elif ncode[0].startswith('#include'):
            if not last_is_include:
                print()
            for l in ncode:
                print(l)
            last_is_include = True
        else:
            last_is_include = False
            print()
            for l in ncode:
                print(l)

if __name__ == '__main__':
    main()
