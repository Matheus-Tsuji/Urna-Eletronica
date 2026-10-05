#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CANDIDATOS 50

#define ID_0 100
#define ID_1 101
#define ID_2 102
#define ID_3 103
#define ID_4 104
#define ID_5 105
#define ID_6 106
#define ID_7 107
#define ID_8 108
#define ID_9 109

#define ID_BRANCO   201
#define ID_CORRIGE  202
#define ID_CONFIRMA 203

typedef struct {
    char numero[4];
    char nome[64];
    char partido[64];
    int votos;
} Candidato;

Candidato listaCandidatos[MAX_CANDIDATOS];
int totalCandidatos = 0;
int votosBrancos = 0;
int votosNulos = 0;

HWND hDisplayTitulo, hDisplayNumero, hDisplayInfo;
char bufferNumero[10] = "";
int votoEmBranco = 0;

HFONT hFonteDigitos, hFonteNome, hFonteRotulo;
HBRUSH hBrushAzulEscuro, hBrushTeclado, hBrushOffWhite;

COLORREF corAzulEscuro = RGB(15, 23, 42);
COLORREF corTeclado    = RGB(24, 24, 27);
COLORREF corOffWhite   = RGB(244, 244, 240);

void carregarCandidatos()
{
    FILE *arquivo = fopen("candidatos.txt", "r");
    totalCandidatos = 0;

    if (!arquivo)
    {
        arquivo = fopen("candidatos.txt", "w");
        if (arquivo)
        {
            fprintf(arquivo, "13;Candidato A;Partido dos Trabalhadores\n");
            fprintf(arquivo, "22;Candidato B;Partido Liberal\n");
            fprintf(arquivo, "30;Candidato C;Partido Novo\n");
            fprintf(arquivo, "55;Candidato D;Partido Social Democratico\n");
            fclose(arquivo);
            arquivo = fopen("candidatos.txt", "r");
        }
    }

    if (arquivo)
    {
        char linha[256];
        while (fgets(linha, sizeof(linha), arquivo) && totalCandidatos < MAX_CANDIDATOS)
        {
            linha[strcspn(linha, "\r\n")] = 0;
            if (strlen(linha) == 0) continue;

            char *num = strtok(linha, ";");
            char *nome = strtok(NULL, ";");
            char *partido = strtok(NULL, ";");

            if (num && nome && partido)
            {
                strncpy(listaCandidatos[totalCandidatos].numero, num, sizeof(listaCandidatos[totalCandidatos].numero) - 1);
                strncpy(listaCandidatos[totalCandidatos].nome, nome, sizeof(listaCandidatos[totalCandidatos].nome) - 1);
                strncpy(listaCandidatos[totalCandidatos].partido, partido, sizeof(listaCandidatos[totalCandidatos].partido) - 1);
                listaCandidatos[totalCandidatos].votos = 0;
                totalCandidatos++;
            }
        }
        fclose(arquivo);
    }
}

void salvarResultadoTxt()
{
    FILE *arquivo = fopen("apuracao_votos.txt", "w");
    if (!arquivo) return;

    fprintf(arquivo, "=== APURACAO DOS VOTOS ===\n\n");
    for (int i = 0; i < totalCandidatos; i++)
    {
        fprintf(arquivo, "[%s] %s (%s): %d votos\n",
                listaCandidatos[i].numero,
                listaCandidatos[i].nome,
                listaCandidatos[i].partido,
                listaCandidatos[i].votos);
    }
    fprintf(arquivo, "\nVOTOS EM BRANCO: %d\n", votosBrancos);
    fprintf(arquivo, "VOTOS NULOS: %d\n", votosNulos);

    fclose(arquivo);
}

Candidato* buscarCandidato(const char* numero)
{
    for (int i = 0; i < totalCandidatos; i++)
    {
        if (strcmp(listaCandidatos[i].numero, numero) == 0)
        {
            return &listaCandidatos[i];
        }
    }
    return NULL;
}

void atualizarTela()
{
    if (votoEmBranco)
    {
        SetWindowText(hDisplayTitulo, "SEU VOTO PARA: PRESIDENTE");
        SetWindowText(hDisplayNumero, "BRANCO");
        SetWindowText(hDisplayInfo, "Aperte CONFIRMA para validar");
        return;
    }

    if (strlen(bufferNumero) == 0)
    {
        SetWindowText(hDisplayTitulo, "SEU VOTO PARA: PRESIDENTE");
        SetWindowText(hDisplayNumero, "_ _");
        SetWindowText(hDisplayInfo, "Digite o numero do candidato");
    }
    else if (strlen(bufferNumero) == 1)
    {
        char exibicao[8];
        sprintf(exibicao, "%s _", bufferNumero);
        SetWindowText(hDisplayNumero, exibicao);
        SetWindowText(hDisplayInfo, "Aguardando segundo digito...");
    }
    else if (strlen(bufferNumero) == 2)
    {
        SetWindowText(hDisplayNumero, bufferNumero);
        Candidato* c = buscarCandidato(bufferNumero);

        if (c != NULL)
        {
            char info[150];
            sprintf(info, "Nome: %s\nPartido: %s", c->nome, c->partido);
            SetWindowText(hDisplayInfo, info);
        }
        else
        {
            SetWindowText(hDisplayInfo, "NUMERO ERRADO\nVOTO NULO");
        }
    }
}

void adicionarDigito(char digito)
{
    if (votoEmBranco) return;
    if (strlen(bufferNumero) < 2)
    {
        int len = strlen(bufferNumero);
        bufferNumero[len] = digito;
        bufferNumero[len + 1] = '\0';
        atualizarTela();
    }
}

void acaoCorrige()
{
    bufferNumero[0] = '\0';
    votoEmBranco = 0;
    atualizarTela();
}

void acaoBranco()
{
    if (strlen(bufferNumero) == 0)
    {
        votoEmBranco = 1;
        atualizarTela();
    }
}

void acaoConfirma(HWND janela)
{
    if (votoEmBranco)
    {
        votosBrancos++;
        salvarResultadoTxt();
        MessageBox(janela, "Voto em BRANCO computado!\n\nFIM", "Urna Eletronica", MB_OK | MB_ICONINFORMATION);
        acaoCorrige();
    }
    else if (strlen(bufferNumero) == 2)
    {
        Candidato* c = buscarCandidato(bufferNumero);
        char msg[256];
        if (c != NULL)
        {
            c->votos++;
            sprintf(msg, "Voto registrado!\n\nCandidato: %s\nPartido: %s\nTotal de votos dele: %d\n\nFIM", c->nome, c->partido, c->votos);
        }
        else
        {
            votosNulos++;
            sprintf(msg, "Voto computado como NULO!\n\nFIM");
        }

        salvarResultadoTxt();
        MessageBox(janela, msg, "Urna Eletronica", MB_OK | MB_ICONINFORMATION);
        acaoCorrige();
    }
    else
    {
        MessageBox(janela, "Complete o numero ou vote em BRANCO!", "Aviso", MB_OK | MB_ICONWARNING);
    }
}

void desenharBotaoCustom(LPDRAWITEMSTRUCT dis)
{
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    char texto[32];
    GetWindowText(dis->hwndItem, texto, sizeof(texto));

    COLORREF bgCor = RGB(30, 30, 30);
    COLORREF bordaCor = RGB(160, 160, 160);
    COLORREF txtCor = RGB(255, 255, 255);

    if (dis->CtlID == ID_BRANCO)
    {
        bgCor = RGB(255, 255, 255);
        bordaCor = RGB(0, 0, 0);
        txtCor = RGB(0, 0, 0);
    }
    else if (dis->CtlID == ID_CORRIGE)
    {
        bgCor = RGB(245, 158, 11);
        bordaCor = RGB(0, 0, 0);
        txtCor = RGB(0, 0, 0);
    }
    else if (dis->CtlID == ID_CONFIRMA)
    {
        bgCor = RGB(34, 197, 94);
        bordaCor = RGB(0, 0, 0);
        txtCor = RGB(0, 0, 0);
    }

    if (dis->itemState & ODS_SELECTED)
    {
        OffsetRect(&rc, 1, 1);
    }

    HBRUSH hBrushBg = CreateSolidBrush(bgCor);
    HPEN hPenBorda = CreatePen(PS_SOLID, 2, bordaCor);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrushBg);
    HPEN oldPen = (HPEN)SelectObject(hdc, hPenBorda);

    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 6, 6);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, txtCor);

    HFONT hFonteBotao = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, hFonteBotao);

    DrawText(hdc, texto, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, oldFont);
    DeleteObject(hFonteBotao);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hBrushBg);
    DeleteObject(hPenBorda);
}

LRESULT CALLBACK WindowProcedure(HWND janela, UINT mensagem, WPARAM wParam, LPARAM lParam)
{
    switch (mensagem)
    {
        case WM_CREATE:
        {
            carregarCandidatos();
            salvarResultadoTxt();

            hFonteDigitos = CreateFont(42, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Consolas");
            hFonteNome    = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
            hFonteRotulo  = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");

            hBrushAzulEscuro = CreateSolidBrush(corAzulEscuro);
            hBrushTeclado    = CreateSolidBrush(corTeclado);
            hBrushOffWhite   = CreateSolidBrush(corOffWhite);

            CreateWindow("STATIC", "JUSTICA ELEITORAL", WS_VISIBLE | WS_CHILD | SS_CENTER, 35, 30, 310, 25, janela, NULL, NULL, NULL);

            hDisplayTitulo = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | SS_LEFT, 50, 65, 280, 25, janela, NULL, NULL, NULL);
            hDisplayNumero = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | SS_CENTER, 50, 110, 280, 55, janela, NULL, NULL, NULL);
            hDisplayInfo   = CreateWindow("STATIC", "", WS_VISIBLE | WS_CHILD | SS_CENTER, 50, 185, 280, 95, janela, NULL, NULL, NULL);

            SendMessage(hDisplayTitulo, WM_SETFONT, (WPARAM)hFonteRotulo, TRUE);
            SendMessage(hDisplayNumero, WM_SETFONT, (WPARAM)hFonteDigitos, TRUE);
            SendMessage(hDisplayInfo,   WM_SETFONT, (WPARAM)hFonteNome, TRUE);

            CreateWindow("BUTTON", "1", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 415, 40, 50, 42, janela, (HMENU)ID_1, NULL, NULL);
            CreateWindow("BUTTON", "2", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 480, 40, 50, 42, janela, (HMENU)ID_2, NULL, NULL);
            CreateWindow("BUTTON", "3", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 545, 40, 50, 42, janela, (HMENU)ID_3, NULL, NULL);

            CreateWindow("BUTTON", "4", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 415, 95, 50, 42, janela, (HMENU)ID_4, NULL, NULL);
            CreateWindow("BUTTON", "5", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 480, 95, 50, 42, janela, (HMENU)ID_5, NULL, NULL);
            CreateWindow("BUTTON", "6", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 545, 95, 50, 42, janela, (HMENU)ID_6, NULL, NULL);

            CreateWindow("BUTTON", "7", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 415, 150, 50, 42, janela, (HMENU)ID_7, NULL, NULL);
            CreateWindow("BUTTON", "8", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 480, 150, 50, 42, janela, (HMENU)ID_8, NULL, NULL);
            CreateWindow("BUTTON", "9", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 545, 150, 50, 42, janela, (HMENU)ID_9, NULL, NULL);

            CreateWindow("BUTTON", "0", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 480, 205, 50, 42, janela, (HMENU)ID_0, NULL, NULL);

            CreateWindow("BUTTON", "BRANCO",   WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 385, 270, 75, 45, janela, (HMENU)ID_BRANCO, NULL, NULL);
            CreateWindow("BUTTON", "CORRIGE",  WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 470, 270, 75, 45, janela, (HMENU)ID_CORRIGE, NULL, NULL);
            CreateWindow("BUTTON", "CONFIRMA", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 555, 260, 85, 58, janela, (HMENU)ID_CONFIRMA, NULL, NULL);

            atualizarTela();
            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(janela, &ps);

            RECT rcJanela;
            GetClientRect(janela, &rcJanela);
            FillRect(hdc, &rcJanela, hBrushAzulEscuro);

            HPEN hPenPreta = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            HPEN oldPen = (HPEN)SelectObject(hdc, hPenPreta);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrushOffWhite);

            RoundRect(hdc, 25, 20, 355, 335, 12, 12);

            SelectObject(hdc, hBrushTeclado);
            RoundRect(hdc, 370, 20, 655, 335, 12, 12);

            SelectObject(hdc, oldBrush);
            SelectObject(hdc, oldPen);
            DeleteObject(hPenPreta);

            EndPaint(janela, &ps);
            break;
        }

        case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            SetBkMode(hdcStatic, TRANSPARENT);
            SetTextColor(hdcStatic, RGB(20, 20, 20));
            return (INT_PTR)hBrushOffWhite;
        }

        case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
            desenharBotaoCustom(dis);
            return TRUE;
        }

        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            if (wmId >= ID_0 && wmId <= ID_9)
            {
                char digito = '0' + (wmId - ID_0);
                adicionarDigito(digito);
            }
            else
            {
                switch (wmId)
                {
                    case ID_BRANCO:   acaoBranco(); break;
                    case ID_CORRIGE:  acaoCorrige(); break;
                    case ID_CONFIRMA: acaoConfirma(janela); break;
                }
            }
            break;
        }

        case WM_DESTROY:
        {
            DeleteObject(hFonteDigitos);
            DeleteObject(hFonteNome);
            DeleteObject(hFonteRotulo);
            DeleteObject(hBrushAzulEscuro);
            DeleteObject(hBrushTeclado);
            DeleteObject(hBrushOffWhite);
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProc(janela, mensagem, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstancia, HINSTANCE hInstanciaAnterior, LPSTR argumentos, int exibirJanela)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = hInstancia;
    wc.lpszClassName = "UrnaEletronicaWin32Custom";
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) return 0;

    HWND janela = CreateWindow(
        "UrnaEletronicaWin32Custom",
        "Urna Eletronica",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        700, 410,
        NULL, NULL, hInstancia, NULL
    );

    if (!janela) return 0;

    ShowWindow(janela, exibirJanela);
    UpdateWindow(janela);

    MSG mensagem;
    while (GetMessage(&mensagem, NULL, 0, 0))
    {
        TranslateMessage(&mensagem);
        DispatchMessage(&mensagem);
    }
    return 0;
}