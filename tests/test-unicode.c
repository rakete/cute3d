#include "stdint.h"
#include "stddef.h"
#include "limits.h"
#include "stdio.h"
#include "stdlib.h"
#include "wchar.h"
#include "locale.h"

#include "driver_log.h"

int main(int argc, char *argv[]) {
    setlocale(LC_ALL,"");

    wprintf(L"type           : size  min/max\n");
    wprintf(L"----------------------------------------------------------------\n");
    wprintf(L"char           :    %zu, %d\n", sizeof(char), CHAR_MIN);
    wprintf(L"wchar_t        :    %zu, %u\n", sizeof(wchar_t), WCHAR_MAX);
    wprintf(L"\n");

    wprintf(L"Hello Unicode!\n");

    const wchar_t* geometric_shapes =
        L"■□▢▣▤▥▦▧▨▩▪▫▬▭▮▯\n" \
        L"▰▱▲△▴▵▶▷▸▹►▻▼▽▾▿\n" \
        L"◀◁◂◃◄◅◆◇◈◉◊○◌◍◎●\n" \
        L"◐◑◒◓◔◕◖◗◘◙◚◛◜◝◞◟\n" \
        L"◠◡◢◣◤◥◦◧◨◩◪◫◬◭◮◯\n";

    wprintf(geometric_shapes);
    for( int i = 0; i < (int)wcslen(geometric_shapes); i++ ) {
        log_assert( geometric_shapes[i] < 65535 );
    }
    wprintf(L"All fine!\n");

    const wchar_t* arrows =
        L"←↑→↓↔↕↖↗↘↙↚↛↜↝↞↟\n" \
        L"↠↡↢↣↤↥↦↧↨↩↪↫↬↭↮↯\n" \
        L"↰↱↲↳↴↵↶↷↸↹↺↻↼↽↾↿\n" \
        L"⇀⇁⇂⇃⇄⇅⇆⇇⇈⇉⇊⇋⇌⇍⇎⇏\n" \
        L"⇐⇑⇒⇓⇔⇕⇖⇗⇘⇙⇚⇛⇜⇝⇞⇟\n" \
        L"⇠⇡⇢⇣⇤⇥⇦⇧⇨⇩⇪\n";

    wprintf(arrows);
    for( int i = 0; i < (int)wcslen(arrows); i++ ) {
        log_assert( arrows[i] < 65535 );
    }
    wprintf(L"All fine!\n");

    return 0;
}
