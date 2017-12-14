#ifndef GEOMETRY_CSG
#define GEOMETRY_CSG

#include "geometry_bsp.h"

//
// Union between ABCD and EFGH
//
//     A                     A
//    +------+              +------+ B1
//    |      | B            |      |
//    |    H |              |      | H1
//    |   +--+---+          |      +---+
//  D |   |  |   | E  =>  D |          | E
//    +---+--+   |          +---+      |
//        | C    |           C1 |      |
//      G |      |              |      |
//        +------+           G1 +--+---+
//              F                F2  F1
//
//  Front <- -> Back
//
//    A               E
//   / \             / \
//  0   B           0   F
//     / \             / \
//    0   C     +     0   G
//       / \             / \
//      0   D           0   H
//         / \             / \
//        0   1           0   1
//
// 1. Insert E in A tree ->
//    E lies behind A ->
//    insert E in B subtree (B is back node) ->
//    E lies in front of B ->
//    empty front node at B ->
//    add E as new B front node!
//
//       A
//      / \
//     0   B
//        / \
//       /   \
//      E     C
//     / \   / \
//    0   1 0   D
//             / \
//            0   1
//
// 2. Insert F in A tree ->
//    F is behind A ->
//    F is cut by B into F1 and F2 ->
//    B is not cut by F ->
//
//    insert F1 into B subtree ->
//    F1 is in front of B ->
//    insert F1 into E subtree ->
//    F1 is behind E ->
//    solid back node at E ->
//    add F1 as new E back node!
//
//    insert F2 into B subtree ->
//    F2 is behind B ->
//    insert F2 into C subtree ->
//    F2 is in front of C ->
//    empty front node at C ->
//    add F2 as new C front node!
//
//
//        A
//       / \
//      0   B
//         / \
//        /   \
//       /     \
//      E       \
//     / \       \
//    0  F1       C
//      /  \     / \
//     0    1   /   \
//             F2    D
//            /  \  / \
//           0    10   1
//
// 3. Insert G in A tree ->
//    G is behind A ->
//    insert G in B subtree ->
//    G is behind B ->
//    insert G in C subtree ->
//    G is cut by C into G1 and G2 ->
//
//    insert G1 in C subtree ->
//    G1 is infront of C ->
//    insert G1 in F2 subtree ->
//    G1 is behind F2 ->
//    empty back node at F2 ->
//    add G1 as new F2 back node!
//
//    insert G2 in C subtree ->
//    G2 is behind C ->
//    insert G2 in D subtree ->
//    G2 is behind D ->
//    empty back node at D ->
//    add G2 as new D back node!
//
//    G1 was inserted into an inside node ->
//    G1 was inserted from the right into right tree ->
//    G1 can NOT be deleted!
//
//    G2 was inserted into an inside node ->
//    G2 was inserted from the right into the left tree ->
//    G2 can be deleted!
//
//        A
//       / \
//      0   B
//         / \
//        /   \
//       /     \
//      E       \
//     / \       \
//    0  F1       C
//      /  \     / \
//     0    1   /   \
//             /     \
//            F2      D
//           /  \    / \
//          0   G1  0  -G2
//             /  \    / \
//            0    1  0   1
//
//    C is cut by G into C1 and C2 ->
//
//    C is from left tree ->
//    replace C with C1 ->
//
//    insert C2 into C subtree ->
//    C2 is in front of C ->
//    insert C2 into F2 subtree ->
//    C2 is behind F2 ->
//    insert C2 into G1 subtree ->
//    C2 is behind G1 ->
//    add C2 as new G1 back node!
//
//    C2 was inserted into an inside node ->
//    C2 was inserted from the left into the right tree ->
//    C2 can be deleted!
//
//        A
//       / \
//      0   B
//         / \
//        /   \
//       /     \
//      E       \
//     / \       \
//    0  F1       C1
//      / \      / \
//     0   1    /   \
//             /     \
//            /       \
//           F2        D
//          / \       / \
//         0   G1    0  -G2
//            / \       / \
//           0   -C2   0   1
//               / \
//              0   1
//
// 4. Insert H in A tree ->
//
//    H is behind A ->
//    H is cut by B into H1 and H2 ->
//
//    insert H1 into B subtree ->
//    H1 is in front of B ->
//    H1 is behind E ->
//    H1 is behind F1 ->
//    add H1 as new F1 back node!
//
//    insert H2 into B subtree ->
//    H2 is behind B ->
//    H2 is behind C1 ->
//    H2 is behind D ->
//    add H2 as new D back node!
//
//    H1 was inserted into an inside node ->
//    H1 was inserted from right into right tree ->
//    H1 can NOT be deleted!
//
//    H2 was inserted into an inside node ->
//    H2 was inserted from right into left tree ->
//    H2 can be deleted!
//
//    B is cut by H into B1 and B2 ->
//
//    B is from left tree ->
//    replace B with B1!
//
//    insert B2 into E subtree ->
//    B2 is behind E ->
//    B2 is behind F1 ->
//    B2 is behind H1 ->
//    add B2 as new H1 back node!
//
//    B2 was inserted into an inside node ->
//    B2 was inserted from left into right tree ->
//    B2 can be deleted!
//
//        A
//       / \
//      0   B1
//         /  \
//        /    \
//       /      \
//      /        \
//     E          \
//    / \          \
//   0   F1         \
//      / \          \
//     0   H1        C1
//        / \        / \
//       0  -B2     /   \
//          / \    /     \
//         0   1  /       \
//               F2        D
//              / \       / \
//             0   G1    0  -G2/-H2
//                / \       / \
//               0  -C2    0   1
//                  / \
//                 0   1
//
// 5. Result ->
//
//        A
//       / \
//      0   B1
//         /  \
//        /    \
//       /      \
//      /        \
//     E          \
//    / \          \
//   0   F1         \
//      / \          \
//     0   H1        C1
//        / \        / \
//       0   1      /   \
//                 /     \
//                /       \
//               F2        D
//              / \       / \
//             0   G1    0   1
//                / \
//               0   1
//
//
//
void csg_union(struct BspTree* a, Mat ab, struct BspTree* b, struct BspTree* r);

//
// Difference between ABCD and EFGH
//
//     A
//    +------+              +------+
//    |      | B            |      |
//    |    H |              |      |
//    |   +--+---+          |   +--+
//  D |   |  |   | E   =>   |   |
//    +---+--+   |          +---+
//        | C    |
//      G |      |
//        +------+
//              F
//
//  Front <- -> Back
//
//    A               E
//   / \             / \
//  0   B           0   F
//     / \             / \
//    0   C     -     0   G
//       / \             / \
//      0   D           0   H
//         / \             / \
//        0   1           0   1
//
// 1. Insert E in A tree ->
//    E lies behind A ->
//    E lies in front of B ->
//    add E as new B front node!
//
//    E was inserted into a front node ->
//    E was inserted from right into left tree ->
//    E can be deleted!
//
//       A
//      / \
//     0   B
//        / \
//       /   \
//     -E     C
//     / \   / \
//    0   1 0   D
//             / \
//            0   1
//
// 2. Insert F in A tree ->
//    F lies behind A ->
//    F is cut by B into F1 and F2 ->
//
//    insert F1 into B subtree ->
//    F1 is in front of B ->
//    add F1 as new B front node!
//
//    insert F2 into B subtree ->
//    F2 is behind B ->
//    F2 is in front of C ->
//    add F2 as new C front node!
//
//    F1 was inserted into a front node ->
//    F1 was inserted from right into left tree ->
//    F1 can be deleted!
//
//    F2 was inserted into a front node ->
//    F2 was inserted from right into left tree ->
//    F2 can be deleted!
//
//        A
//       / \
//      0   B
//         / \
//        /   \
//       /     \
//    -E/-F1    \
//               \
//                C
//               / \
//              /   \
//            -F2    D
//                  / \
//                 0   1
//
// 3. Insert G in A tree ->
//    F lies behind A ->
//    G lies behind B ->
//    G is cut by C into G1 and G2 ->
//
//    insert G1 into C subtree ->
//    G1 is in front of C ->
//    add G1 as new C front node!
//
//    insert G2 into C subtree ->
//    G2 is behind C ->
//    G2 is behind D ->
//    add G2 as new D back node!
//
//    G1 was inserted into a front node ->
//    G1 was inserted from right into left tree ->
//    G1 can be deleted!
//
//    G2 was inserted into a back node ->
//    G2 can NOT be deleted!
//
//    C is cut by G into C1 and C2 ->
//
//    C is from left tree ->
//    replace C with C1!
//
//    insert C2 into D subtree ->
//    C2 is in front of G2 ->
//    add C2 as new G2 front node!
//
//    C2 was inserted into a front node ->
//    C2 was inserted from right into left tree ->
//    C2 can be deleted!
//
//
//        A
//       / \
//      0   B
//         / \
//        /   \
//       /     \
//    -E/-F1    \
//               \
//                C1
//               / \
//              /   \
//          -F2/-G1  D
//                  / \
//                 0   G2
//                    / \
//                  -C2  1
//
// 4. Insert H in A tree ->
//    H lies behind A ->
//    H is cut by B into H1 and H2 ->
//
//    insert H1 into B subtree ->
//    H1 is in front of B ->
//    add H1 as new B front node!
//
//    insert H2 into B subtree ->
//    H2 is behind B ->
//    H2 is behind C1 ->
//    H2 is behind D ->
//    H2 is in front of G2 ->
//    add H2 as new G front node!
//
//    H1 was inserted into a front node ->
//    H1 was inserted from right into left tree ->
//    H1 can be deleted!
//
//    H2 was inserted into a front node ->
//    H2 was inserted from right into right tree ->
//    H1 can NOT be deleted!
//
//    B is cut by H into B1 and B2 ->
//
//    B is from left tree ->
//    replace B with B1!
//
//    insert B2 into C1 subtree ->
//    B2 is behind C1 ->
//    B2 is behind D ->
//    B2 in front of G2 ->
//    B2 in front of H2 ->
//    add B2 as new H2 front node!
//
//    B2 was inserted into a front node ->
//    B2 was inserted from left into right tree ->
//    B2 can be deleted!
//
//
//
//        A
//       / \
//      0   B1
//         / \
//        /   \
//       /     \
//  -E/-F1/-H1  \
//               \
//                C1
//               / \
//              /   \
//          -F2/-G1  D
//                  / \
//                 0   G2
//                    / \
//                   /   1
//               -C2/H2
//                 / \
//               -B2  1
//
// 5. Result ->
//
//        A
//       / \
//      0   B1
//         / \
//        0   C1
//           / \
//          0   D
//             / \
//            0   G2
//               / \
//              H2  1
//             / \
//            0   1
//
void csg_difference(struct BspTree* a, Mat ab, struct BspTree* b, struct BspTree* r);

//
// Intersection between ABCD and EFGH
//
//     A
//    +------+
//    |      | B
//    |    H |
//    |   +--+---+            +--+
//  D |   |  |   | E    =>    |  |
//    +---+--+   |            +--+
//        | C    |
//      G |      |
//        +------+
//              F
//
//  Front <- -> Back
//
//    A               E
//   / \             / \
//  0   B           0   F
//     / \             / \
//    0   C     v     0   G
//       / \             / \
//      0   D           0   H
//         / \             / \
//        0   1           0   1
//
//
void csg_intersection(struct BspTree* a, Mat ab, struct BspTree* b, struct BspTree* r);

#endif
