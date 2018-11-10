#include <protocol/RouterTable.h>
#include <iostream>
#include <bitset>

#define GENIP(a) IP i##a(a,s##a)

void testHuffman() {
    Huffman h;
    in_addr a,b,c,d,sa,sb,sc,sd;
    inet_aton("0.0.0.0", &a);
    inet_aton("0.0.0.0", &b);
    inet_aton("0.0.0.0", &c);
    inet_aton("0.0.0.0", &d);
    inet_aton("255.0.0.0", &sa);
    inet_aton("255.128.0.0", &sb);
    inet_aton("255.192.0.0", &sc);
    inet_aton("255.224.0.0", &sd);
    GENIP(a);
    GENIP(b);
    GENIP(c);
    GENIP(d);
    h.insert(ib, 2);
    h.printTree();
    h.insert(ic, 3);
    h.printTree();
    printf("%d\n", h.query(ia));
    printf("%d\n", h.query(ib));
    printf("%d\n", h.query(ic));
    printf("%d\n", h.query(id));
}

void testRouterTable() {
    RouterTable t(4);
    in_addr a,aa,b,c,d,e,g,sa,saa,sb,sc,sd,se,sg;
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

    RouterInfo ar = RouterInfo(0, av, std::vector<uint16_t>({1,2}),1000);
    RouterInfo br = RouterInfo(1, bv, std::vector<uint16_t>({0,3,5}),1000);
    RouterInfo cr = RouterInfo(2, cv, std::vector<uint16_t>({0,4}),1000);
    RouterInfo dr = RouterInfo(3, dv, std::vector<uint16_t>({1,4}),1000);
    RouterInfo er = RouterInfo(4, ev, std::vector<uint16_t>({2,3,5}),1000);
    RouterInfo gr = RouterInfo(5, gv, std::vector<uint16_t>({1,4}),1000);
    RouterInfo ggr = RouterInfo(5, gv, std::vector<uint16_t>({1,4}),998);
    RouterInfo gggr = RouterInfo(5, gv, std::vector<uint16_t>(),1002);
    t.update(ar);
    t.update(br);
    t.update(cr);
    t.update(dr);
    t.update(er);
    t.update(gr);
    t.update(ggr);
    t.update(gggr);
    t.updateStartPoint(0,std::vector<uint16_t>({2}));
    t.updateStartPoint(1,std::vector<uint16_t>({3}));
    t.updateStartPoint(2,std::vector<uint16_t>({5}));
    printf("%d\n", t.query(ia));
    printf("%d\n", t.query(ib));
    printf("%d\n", t.query(ic));
    printf("%d\n", t.query(id));
    printf("%d\n", t.query(ie));
    printf("%d\n", t.query(ig));
}
int main() {
    testHuffman();
    testRouterTable();
}

