#!/usr/bin/python3

def formatmacro(macro):
    s = 0
    ret = []
    for line in macro:
        if line.strip().startswith('}') or line.strip().startswith('})'):
            s -= 1
        toappend = '    ' * s + line
        toappend += max(79 - len(toappend), 2) * ' ' + '\\'
        ret.append(toappend)
        if line.strip().endswith('{'):
            s += 1
    ret[-1] = ret[-1][:-1].rstrip()
    return ret

class Header:
    def __init__(self):
        self.name = None
        self.include = []
        self.funcs = []
    def read_file(self, filename):
        with open(filename) as f:
            code = list(f)
        pos = 0
        while pos != len(code):
            pos, t, op = read_next(code, pos)
            if t == 'filename':
                self.name = op
            if t == 'include':
                self.include.append(op)
            if t == 'func':
                self.funcs.append(op)
    def __str__(self):
        ret = ""
        ret += '#ifndef ' + self.name.replace('.', '_').upper() + '\n'
        ret += '#define ' + self.name.replace('.', '_').upper() + '\n\n'

        for i in self.include:
            ret += '#include ' + i + '\n'
        ret += '#include <utils/errorutils.h>\n'
        ret += '\n'

        for f in self.funcs:
            ret += str(f)
            ret += '\n\n'

        ret += '#endif // ' + self.name.replace('.', '_').upper() + '\n'
        return ret


class Func:
    def __init__(self):
        self.t = None
        self.name = None
        self.arg = None
        self.max_vaarg = 0
        self.body = None
    
    def initial_macro(self):
        return formatmacro(['#define ' + self.name + '(' + ', '.join(self.arg) + ', ...) ({',
                            'ErrorBehavior _ebo = ErrorBehavior("");',
                            '_ebo.from = SOURCE' + '_' + self.t.upper() + ';',
                            '_' + self.name + '0(' + ', '.join(self.arg) + ', ##__VA_ARGS__, _ebo' + ', 0' * (self.max_vaarg + 2) + ');',
                            '})'])
    def after_macro(self):
        formatted = []
        for i in range(self.max_vaarg + 1):
            lst = []
            fstline = '#define _' + self.name + str(i) + '(' + ', '.join(self.arg)
            for j in range(i):
                fstline += ', _a' + str(j)
            fstline += ', _behavior, _action, ...) ({'
            dispatchlines = ['decltype(({_behavior;})) *_decleb;',
                             'if(std::is_same<ErrorBehavior, std::remove_pointer<decltype(_decleb)>::type>()) {']
            workinglines = ['ErrorBehavior _eb' + str(i) + ' = behaviorFrom(({_behavior;}));',
                            '_eb' + str(i) + '.sys_msg = "' + self.rawname + '";']
            decllines = []
            retlines = []
            def geterrorexpr(l, i):
                ret = []
                if self.t == 'pcap_buffer':
                    errbuf = l[4:].strip()
                    ret.append('_eb' + str(i) + '.buf = ' + errbuf + ';')
                #ret.append('if (isErrorBehavior(_action)) {')
                #ret.append('ERROR_WITH_BEHAVIOR(_eb' + str(i) + ', exit(0));')
                #ret.append('} else {')
                ret.append('ERROR_WITH_BEHAVIOR(_eb' + str(i) + ', _action);')
                #ret.append('}')
                return ret
            for l in self.body:
                stripl = l.strip()
                if stripl.startswith('@decl'):
                    decllines.append(stripl[5:].strip() + ';')
                    continue
                if stripl.startswith('@ret') and not stripl.startswith('@reteq'):
                    retlines.append(stripl[4:].strip() + ';')
                    continue
                if stripl.startswith('@err'):
                    workinglines.extend(geterrorexpr(l, i))
                    continue
                if stripl.find('@call') != -1:
                    pos = stripl.find('@call')
                    callingstr = stripl[0:pos] + self.rawname + '(' + ', '.join(self.arg)
                    for j in range(i):
                        callingstr += ', _a' + str(j)
                    callingstr += ')'
                    callingstr += stripl[pos + 5:]
                    workinglines.append(callingstr)
                    continue
                if stripl.startswith('@pcap'):
                    ptr = stripl[5:].strip();
                    workinglines.append('_eb' + str(i) + '.pcap = ' + ptr + ';')
                    continue
                if stripl.startswith('@noreteq'):
                    callingstr = 'if (' + self.rawname + '(' + ', '.join(self.arg)
                    for j in range(i):
                        callingstr += ', _a' + str(j)
                    callingstr += ') == '
                    eqarg = stripl[8:].strip()
                    callingstr += eqarg
                    callingstr += ') {'
                    workinglines.append(callingstr)
                    workinglines.extend(geterrorexpr(l, i))
                    workinglines.append('}')
                    continue
                if stripl.startswith('@noretneq'):
                    callingstr = 'if (' + self.rawname + '(' + ', '.join(self.arg)
                    for j in range(i):
                        callingstr += ', _a' + str(j)
                    callingstr += ') != '
                    eqarg = stripl[9:].strip()
                    callingstr += eqarg
                    callingstr += ') {'
                    workinglines.append(callingstr)
                    workinglines.extend(geterrorexpr(l, i))
                    workinglines.append('}')
                    continue
                if stripl.startswith('@reteq'):
                    params = list(filter(lambda x: x != '', stripl[6:].split(' ')))
                    eqarg = params[0]
                    retarg = params[1]
                    callingstr = 'if ((' + retarg + ' = ' + self.rawname + '(' + ', '.join(self.arg)
                    for j in range(i):
                        callingstr += ', _a' + str(j)
                    callingstr += ')) == '
                    callingstr += eqarg
                    callingstr += ') {'
                    workinglines.append(callingstr)
                    workinglines.extend(geterrorexpr(l, i))
                    workinglines.append('}')
                    retlines.append(retarg + ';')
                    continue
                if stripl.startswith('@retneq'):
                    params = list(filter(lambda x: x != '', stripl[6:].split(' ')))
                    eqarg = params[0]
                    retarg = params[1]
                    callingstr = 'if ((' + retarg + ' = ' + self.rawname + '(' + ', '.join(self.arg)
                    for j in range(i):
                        callingstr += ', _a' + str(j)
                    callingstr += ')) != '
                    callingstr += eqarg
                    callingstr += ') {'
                    workinglines.append(callingstr)
                    workinglines.extend(geterrorexpr(l, i))
                    workinglines.append('}')
                    retlines.append(retarg + ';')
                    continue
                workinglines.append(stripl)
            if i != self.max_vaarg:
                dispatchfaillines = ['} else {',
                                     '_' + self.name + str(i+1) + '(' + ', '.join(self.arg),
                                     '}']
                for j in range(i):
                    dispatchfaillines[1] += ', ' + '_a' + str(j)
                dispatchfaillines[1] += ', _behavior, _action, ##__VA_ARGS__);'
            else:
                dispatchfaillines = ['} else {',
                                     'printf("Too many args for ' + self.name + '\\n");',
                                     'exit(-1);',
                                     '}']
            closeline = '})'
            lst.append(fstline)
            lst.extend(decllines)
            lst.extend(dispatchlines)
            lst.extend(workinglines)
            lst.extend(dispatchfaillines)
            lst.extend(retlines)
            lst.append(closeline)
            formatted.extend(formatmacro(lst))
        return formatted
    def __str__(self):
        return '\n'.join(self.initial_macro() + self.after_macro())


def read_body(text, pos):
    b = []
    while pos < len(text) and not text[pos].strip().endswith('@endbody'):
        l = text[pos]
        if l.strip().startswith('@body'):
            pos += 1
            continue
        b.append(text[pos])
        pos += 1
    return (pos, b)

def read_func(text, pos):
    f = Func()
    while pos < len(text) and not text[pos].strip().startswith('@end'):
        l = text[pos]
        if l.startswith('@type'):
            f.t = l[5:].strip()
        if l.startswith('@name'):
            f.rawname = l[5:].strip()
            f.name = f.rawname.capitalize()
        if l.startswith('@arg'):
            args = list(filter(lambda x: not x == '', l[5:].split()))
            f.arg = args
        if l.startswith('@vaarg'):
            f.max_vaarg = int(l[6:])
        if l.startswith('@body'):
            pos, f.body = read_body(text, pos)
        if l.startswith('@end'):
            pos += 1
            break;
        pos += 1
    return (pos, f)

def read_next(text, pos):
    l = text[pos]
    if l.strip() == '':
        return (pos + 1, 'space', None);
    if l.strip().startswith('@filename'):
        filename = l[9:].strip()
        return (pos + 1, 'filename', filename)
    if l.strip().startswith('@include'):
        includename = l[8:].strip()
        return (pos + 1, 'include', includename)
    if l.strip().startswith('@type'):
        pos, f = read_func(text, pos)
        return (pos + 1, 'func', f)


def main():
    name = input()
    #name = './include/wrapper/pcap.h.raw'
    h = Header()
    l = name.split('/')[:-1]
    h.read_file(name)
    l.append(h.name)
    outfilename = '/'.join(l)
    print(h)
    with open(outfilename, 'w') as f:
        f.write(str(h))


if __name__ == '__main__':
    main()
