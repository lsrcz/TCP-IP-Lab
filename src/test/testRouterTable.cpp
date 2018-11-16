#include <bitset>
#include <iostream>
#include <protocol/RouterTable.h>

#define GENIP(a) IP i##a(a, s##a)

void testHuffman() {
    Huffman h;
    in_addr a, b, c, d, e, sa, sb, sc, sd, se;
    inet_aton("192.192.34.0", &a);
    inet_aton("192.192.34.0", &b);
    inet_aton("192.64.34.0", &c);
    inet_aton("192.64.34.1", &d);
    inet_aton("192.64.34.2", &e);
    inet_aton("255.0.0.0", &sa);
    inet_aton("255.128.0.0", &sb);
    inet_aton("255.192.0.0", &sc);
    inet_aton("255.224.0.0", &sd);
    inet_aton("255.255.255.255", &se);
    GENIP(a);
    GENIP(b);
    GENIP(c);
    GENIP(d);
    GENIP(e);
    h.insert(ib, b, 2);
    h.printTree();
    h.insert(ic, c, 3);
    h.printTree();
    h.insert(ie, e, 4);
    h.printTree();
    in_addr x;
    int     p;
    p = h.query(a, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
    p = h.query(b, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
    p = h.query(c, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
    p = h.query(d, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
    p = h.query(e, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
}

void testRouterTable() {
    RouterTable t;
    t.setStart(4);
    in_addr a, aa, b, c, d, e, g, sa, saa, sb, sc, sd, se, sg;
    inet_aton("0.0.0.0", &a);
    inet_aton("0.0.0.6", &aa);
    inet_aton("0.0.0.1", &b);
    inet_aton("0.0.0.2", &c);
    inet_aton("0.0.0.3", &d);
    inet_aton("0.0.0.4", &e);
    inet_aton("0.0.0.5", &g);
    inet_aton("255.255.255.255", &sa);
    inet_aton("255.255.255.255", &saa);
    inet_aton("255.255.255.255", &sb);
    inet_aton("255.255.255.255", &sc);
    inet_aton("255.255.255.255", &sd);
    inet_aton("255.255.255.255", &se);
    inet_aton("255.255.255.255", &sg);
    GENIP(a);
    GENIP(aa);
    GENIP(b);
    GENIP(c);
    GENIP(d);
    GENIP(e);
    GENIP(g);
    std::vector<IP> av;
    av.push_back(ia);
    av.push_back(iaa);
    std::vector<IP> bv;
    bv.push_back(ib);
    std::vector<IP> cv;
    cv.push_back(ic);
    std::vector<IP> dv;
    dv.push_back(id);
    std::vector<IP> ev;
    ev.push_back(ie);
    std::vector<IP> gv;
    gv.push_back(ig);

    RouterInfo ar = RouterInfo(0, av, std::vector<uint16_t>({ 1, 2 }), 1000);
    RouterInfo br = RouterInfo(1, bv, std::vector<uint16_t>({ 0, 3, 5 }), 1000);
    RouterInfo cr = RouterInfo(2, cv, std::vector<uint16_t>({ 0, 4 }), 1000);
    RouterInfo dr = RouterInfo(3, dv, std::vector<uint16_t>({ 1, 4 }), 1000);
    RouterInfo er = RouterInfo(4, ev, std::vector<uint16_t>({ 2, 3, 5 }), 1000);
    RouterInfo gr = RouterInfo(5, gv, std::vector<uint16_t>({ 1, 4 }), 1000);
    RouterInfo ggr  = RouterInfo(5, gv, std::vector<uint16_t>({ 1, 4 }), 998);
    RouterInfo gggr = RouterInfo(5, gv, std::vector<uint16_t>(), 1002);
    t.update(ar);
    t.update(br);
    t.update(cr);
    t.update(dr);
    t.update(er);
    t.update(gr);
    t.update(ggr);
    t.update(gggr);
    std::pair<uint16_t, in_addr> y = std::make_pair(2, c);
    t.updateStartPoint(2, std::set<std::pair<uint16_t, in_addr>>({ y }));
    y = std::make_pair(3, d);
    t.updateStartPoint(3, std::set<std::pair<uint16_t, in_addr>>({ y }));
    y = std::make_pair(5, g);
    t.updateStartPoint(5, std::set<std::pair<uint16_t, in_addr>>({ y }));
    in_addr x;
    int     p;
    p = t.query(a, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
    p = t.query(b, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
    p = t.query(c, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
    p = t.query(d, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
    p = t.query(e, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
    p = t.query(g, &x);
    printf("%d,%s\n", p, inet_ntoa(x));
    t.printRouterTable();
}
int main() {
    testHuffman();
    testRouterTable();
}
