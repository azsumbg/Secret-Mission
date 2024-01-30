#include "framework.h"
#include "Secret Mission.h"
#include "mmsystem.h"
#include "smhelper.h"
#include "ErrH.h"
#include "FCheck.h"
#include "d2d1.h"
#include "dwrite.h"
#include "D2BMPLOADER.h"
#include <vector>
#include <ctime>
#include <chrono>
#include <fstream>

#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "errh.lib")
#pragma comment (lib, "fcheck.lib")
#pragma comment (lib, "smhelper.lib")
#pragma comment (lib, "d2bmploader.lib")

#define bWinClassName L"MySecretMission"

#define mNew 1001
#define mSpeed 1002
#define mExit 1003
#define mSave 1004
#define mLoad 1005
#define mHoF 1006

#define tmpFile ".\\res\\data\\temp.dat"
#define LtmpFile L".\\res\\data\\temp.dat"
#define saveFile L".\\res\\data\\save.dat"
#define recordFile L".\\res\\data\\record.dat"
#define helpFile L".\\res\\data\\help.dat"
#define soundFile L".\\res\\snd\\main.wav"

#define record 2001
#define no_record 2002
#define first_record 2003

////////////////////////////////////////////

WNDCLASS bWin = { 0 };
HINSTANCE bIns = nullptr;
HWND bHwnd = nullptr;
HICON mainIcon = nullptr;
HCURSOR mainCursor = nullptr;
HCURSOR outCursor = nullptr;
HMENU bBar = nullptr;
HMENU bMain = nullptr;
HMENU bStore = nullptr;
HDC PaintDC = nullptr;
PAINTSTRUCT bPaint = { 0 };
MSG bMsg = { 0 };
BOOL bRet = 0;
POINT cur_pos = { 0 };
UINT bTimer = 0;

D2D1_RECT_F b1Rct = { 0,0,300.0f,50.0f };
D2D1_RECT_F b2Rct = { 350.0f,0,650.0f,50.0f };
D2D1_RECT_F b3Rct = { 700.0f,0,scr_width,50.0f };

int level = 1;
int score = 0;
int minutes = 0;
int seconds = 180;

float cl_width = 0;
float cl_height = 0;

wchar_t current_player[16] = L"A SPY";

bool pause = false;
bool show_help = false;
bool sound = true;
bool in_client = true;
bool set_name = false;
bool b1_hglt = false;
bool b2_hglt = false;
bool b3_hglt = false;

////////////////////////////////////////////////////

ID2D1Factory* iFactory = nullptr;
ID2D1HwndRenderTarget* Draw = nullptr;

IDWriteFactory* iWriteFactory = nullptr;
IDWriteTextFormat* nrmTextForm = nullptr;
IDWriteTextFormat* bigTextForm = nullptr;

ID2D1LinearGradientBrush* but_bck_brush = nullptr;
ID2D1SolidColorBrush* but_txt_brush = nullptr;
ID2D1SolidColorBrush* but_hglt_brush = nullptr;
ID2D1SolidColorBrush* but_inact_brush = nullptr;
ID2D1SolidColorBrush* field_txt_brush = nullptr;

ID2D1Bitmap* bmpBadLaser = nullptr;
ID2D1Bitmap* bmpLaser = nullptr;
ID2D1Bitmap* bmpBadShip = nullptr;
ID2D1Bitmap* bmpBigBlockU = nullptr;
ID2D1Bitmap* bmpBigBlockD = nullptr;
ID2D1Bitmap* bmpSmallBlock = nullptr;
ID2D1Bitmap* bmpGreatBlock = nullptr;
ID2D1Bitmap* bmpCloud1 = nullptr;
ID2D1Bitmap* bmpCloud2 = nullptr;
ID2D1Bitmap* bmpPortal = nullptr;

ID2D1Bitmap* bmpExplosion[10] = { 0 };
ID2D1Bitmap* bmpField[24] = { 0 };
ID2D1Bitmap* bmpShipL[13] = { 0 };
ID2D1Bitmap* bmpShipR[13] = { 0 };

//////////////////////////////////////

prot_ptr Field = nullptr;


///////////////////////////////////////

template <typename PTR> bool ReleaseCOM(PTR** Object)
{
    if ((*Object))
    {
        (*Object)->Release();
        (*Object) = nullptr;
        return false;
    }
    return true;
}
void ErrLog(LPCWSTR txt)
{
    std::wofstream err_file(L".\\res\\data\\err.log", std::ios::app);
    err_file << txt << L" ! at: " << std::chrono::system_clock::now() << std::endl;
    err_file.close();
}
bool GarbageCollector()
{
    bool err = false;

    err = ReleaseCOM(&iFactory);
    if (err)ErrLog(L"Error in RelaseCOM(&iFactory)");

    err = ReleaseCOM(&Draw);
    if (!err)ErrLog(L"Error in RelaseCOM(&Draw)");

    err = ReleaseCOM(&iWriteFactory);
    if (err)ErrLog(L"Error in RelaseCOM(&iWriteFactory)");

    err = ReleaseCOM(&nrmTextForm);
    if (err)ErrLog(L"Error in RelaseCOM(&nrmTextForm)");

    err = ReleaseCOM(&bigTextForm);
    if (err)ErrLog(L"Error in RelaseCOM(&bigTextForm)");

    err = ReleaseCOM(&but_bck_brush);
    if (err)ErrLog(L"Error in RelaseCOM(&but_bckg_brush)");

    err = ReleaseCOM(&but_hglt_brush);
    if (err)ErrLog(L"Error in RelaseCOM(&but_hglt_brush)");

    err = ReleaseCOM(&but_txt_brush);
    if (err)ErrLog(L"Error in RelaseCOM(&but_txt_brush)");

    err = ReleaseCOM(&but_inact_brush);
    if (err)ErrLog(L"Error in RelaseCOM(&but_inact_brush)");

    err = ReleaseCOM(&field_txt_brush);
    if (err)ErrLog(L"Error in RelaseCOM(&field_txt_brush)");

    err = ReleaseCOM(&bmpBadLaser);
    if (err)ErrLog(L"Error in RelaseCOM(&bmpBadLaser)");

    err = ReleaseCOM(&bmpLaser);
    if (err)ErrLog(L"Error in RelaseCOM(&bmpLaser)");

    err = ReleaseCOM(&bmpBadShip);
    if (err)ErrLog(L"Error in RelaseCOM(&bmpBadShip)");

    err = ReleaseCOM(&bmpBigBlockD);
    if (err)ErrLog(L"Error in RelaseCOM(&bmpBigBlockD)");

    err = ReleaseCOM(&bmpBigBlockU);
    if (err)ErrLog(L"Error in RelaseCOM(&bmpBigBlockU)");

    err = ReleaseCOM(&bmpCloud1);
    if (err)ErrLog(L"Error in RelaseCOM(&bmpCloud1)");

    err = ReleaseCOM(&bmpCloud2);
    if (err)ErrLog(L"Error in RelaseCOM(&bmpCloud2)");

    err = ReleaseCOM(&bmpSmallBlock);
    if (err)ErrLog(L"Error in RelaseCOM(&bmpSmallBlock)");

    err = ReleaseCOM(&bmpGreatBlock);
    if (err)ErrLog(L"Error in RelaseCOM(&bmpGreatBlock)");

    err = ReleaseCOM(&bmpPortal);
    if (err)ErrLog(L"Error in RelaseCOM(&bmpPortal)");

    for (int i = 0; i < 10; i++)
    {
        err = ReleaseCOM(&bmpExplosion[i]);
        if (err)
        {
            wchar_t errtxt[60] = L"Error in RelaseCOM(&bmpExplosion), frame: ";
            wchar_t add[3] = L"\0";
            swprintf(add, 3, L"%d", i);
            wcscat_s(errtxt, add);
            ErrLog(errtxt);
        }
    }

    for (int i = 0; i < 24; i++)
    {
        err = ReleaseCOM(&bmpField[i]);
        if (err)
        {
            wchar_t errtxt[60] = L"Error in RelaseCOM(&bmpField), frame: ";
            wchar_t add[3] = L"\0";
            swprintf(add, 3, L"%d", i);
            wcscat_s(errtxt, add);
            ErrLog(errtxt);
        }
    }

    for (int i = 0; i < 13; i++)
    {
        err = ReleaseCOM(&bmpShipL[i]);
        if (err)
        {
            wchar_t errtxt[60] = L"Error in RelaseCOM(&bmpShipL), frame: ";
            wchar_t add[3] = L"\0";
            swprintf(add, 3, L"%d", i);
            wcscat_s(errtxt, add);
            ErrLog(errtxt);
        }
    }

    for (int i = 0; i < 13; i++)
    {
        err = ReleaseCOM(&bmpShipR[i]);
        if (err)
        {
            wchar_t errtxt[60] = L"Error in RelaseCOM(&bmpShipR), frame: ";
            wchar_t add[3] = L"\0";
            swprintf(add, 3, L"%d", i);
            wcscat_s(errtxt, add);
            ErrLog(errtxt);
        }
    }

    return err;
}
void ErrExit(int which_error, LPCWSTR log_txt = L"\0")
{
    if (which_error == eD2D)ErrLog(log_txt);

    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(which_error), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
    
    GarbageCollector();
    std::remove(tmpFile);
    exit(1);
}
void InitGame()
{
    level = 1;
    score = 0;
    minutes = 0;
    seconds = 180;
    wcscat_s(current_player, L"A SPY");
    set_name = false;

    ReleaseCOM(&Field);

    Field = ProtonFactory(types::field, 0, 0);


}

void GameOver()
{
    KillTimer(bHwnd, bTimer);
    PlaySound(NULL, NULL, NULL);


    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}
void InitD2D1()
{
    HRESULT hr = S_OK;

    D2D1_GRADIENT_STOP gStop[2] = { 0 };
    ID2D1GradientStopCollection* gStopCollection = nullptr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating iFactory");

    hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(bHwnd, D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating Draw");

    gStop[0].position = 0;
    gStop[0].color = D2D1::ColorF(D2D1::ColorF::AliceBlue);
    gStop[1].position = 1.0f;
    gStop[1].color = D2D1::ColorF(D2D1::ColorF::DarkCyan);

    hr = Draw->CreateGradientStopCollection(gStop, 2, &gStopCollection);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating gStopCollection");

    if (gStopCollection)
        hr = Draw->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), 
            D2D1::Point2F(scr_width, 50.0f)), gStopCollection, &but_bck_brush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating but_bck_brush");

    ReleaseCOM(&gStopCollection);

    hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkBlue), &but_txt_brush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating but_txt_brush");

    hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GreenYellow), &but_hglt_brush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating but_hglt_brush");

    hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &but_inact_brush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating but_inact_brush");

    hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &field_txt_brush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating field_txt_brush");
    
    // WRITE FACTORY ******************************

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&iWriteFactory));
    if (hr != S_OK)ErrExit(eD2D, L"Error creating iWriteFactory");

    hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 28, L"", &nrmTextForm);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating nrmTextFormat");

    hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
        DWRITE_FONT_STRETCH_EXTRA_EXPANDED, 64, L"", &bigTextForm);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating bigTextFormat");

    ///////////////////////////////////////////////

    bmpBadLaser = Load(L".\\res\\img\\bad_laser.png", Draw);
    if (!bmpBadLaser)ErrExit(eD2D, L"Error loading bmpBadLaser");

    bmpBadShip = Load(L".\\res\\img\\battleship.png", Draw);
    if (!bmpBadShip)ErrExit(eD2D, L"Error loading bmpBadShip");

    bmpBigBlockD = Load(L".\\res\\img\\big_block_d.png", Draw);
    if (!bmpBigBlockD)ErrExit(eD2D, L"Error loading bmpBigBlockD");

    bmpBigBlockU = Load(L".\\res\\img\\big_block.png", Draw);
    if (!bmpBigBlockU)ErrExit(eD2D, L"Error loading bmpBigBlockU");

    bmpCloud1 = Load(L".\\res\\img\\cloud1.png", Draw);
    if (!bmpCloud1)ErrExit(eD2D, L"Error loading bmpCloud1");

    bmpCloud2 = Load(L".\\res\\img\\cloud1.png", Draw);
    if (!bmpCloud2)ErrExit(eD2D, L"Error loading bmpCloud2");

    bmpGreatBlock = Load(L".\\res\\img\\v_block.png", Draw);
    if (!bmpGreatBlock)ErrExit(eD2D, L"Error loading bmpGreatBlock");

    bmpPortal = Load(L".\\res\\img\\portal.png", Draw);
    if (!bmpPortal)ErrExit(eD2D, L"Error loading bmpPortal");

    bmpSmallBlock = Load(L".\\res\\img\\small_block.png", Draw);
    if (!bmpSmallBlock)ErrExit(eD2D, L"Error loading bmpSmallBlock");

    bmpLaser = Load(L".\\res\\img\\laser.png", Draw);
    if (!bmpLaser)ErrExit(eD2D, L"Error loading bmpLaser");

    for (int i = 0; i < 23; i++)
    {
        wchar_t name[100] = L".\\res\\img\\explosion\\";
        wchar_t add[5] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpExplosion[i] = Load(name, Draw);
        if (!bmpExplosion[i])ErrExit(eD2D, L"Error loading bmpExplosion");
    }

    for (int i = 0; i < 10; i++)
    {
        wchar_t name[100] = L".\\res\\img\\field\\";
        wchar_t add[5] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpField[i] = Load(name, Draw);
        if (!bmpField[i])ErrExit(eD2D, L"Error loading bmpField");
    }

    for (int i = 0; i < 13; i++)
    {
        wchar_t name[100] = L".\\res\\img\\ship_l\\";
        wchar_t add[5] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpShipL[i] = Load(name, Draw);
        if (!bmpShipL[i])ErrExit(eD2D, L"Error loading bmpShipL");
    }

    for (int i = 0; i < 13; i++)
    {
        wchar_t name[100] = L".\\res\\img\\ship_r\\";
        wchar_t add[5] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");

        bmpShipR[i] = Load(name, Draw);
        if (!bmpShipR[i])ErrExit(eD2D, L"Error loading bmpShipR");
    }

    //////////////////////////////////////////

    wchar_t start_text[30] = L"СЕКРЕТНА МИСИЯ !\n\ndev. Daniel";
    wchar_t show_text[30] = L"\0";

    for (int i = 0; i < 30; i++)
    {
        mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
        show_text[i] = start_text[i];
        if (Draw && bigTextForm && field_txt_brush)
        {
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkBlue));
            Draw->DrawTextW(show_text, i, bigTextForm, D2D1::RectF(300.0f, 300.0f, cl_width, cl_height), field_txt_brush);
            Draw->EndDraw();
            Sleep(30);
        }
    }
    Sleep(2500);
}

INT_PTR CALLBACK bDlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch(ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(mainIcon));
        return true;
        break;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
            {
                int size = GetDlgItemText(hwnd, IDC_NAME, current_player, 15);
                if (size < 1)
                {
                    if (sound)MessageBeep(MB_ICONEXCLAMATION);
                    MessageBox(bHwnd, L"Името си ли забрави ?", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONQUESTION);
                    wcscat_s(current_player, L"A SPY");
                    EndDialog(hwnd, IDCANCEL);
                }
                EndDialog(hwnd, IDOK);
            }
            break;
        }
        break;

    }

    return (INT_PTR)(FALSE);
}
LRESULT CALLBACK bWinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        {
            srand((unsigned int)(time(0)));
            SetTimer(hwnd, bTimer, 1000, NULL);
            RECT rct = { 0 };
            GetClientRect(hwnd, &rct);
            cl_width = static_cast<float>(rct.right);
            cl_height = static_cast<float>(rct.bottom);
            
            bBar = CreateMenu();
            bMain = CreateMenu();
            bStore = CreateMenu();

            AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
            AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

            AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
            AppendMenu(bMain, MF_STRING, mSpeed, L"Следващо ниво");
            AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
            AppendMenu(bMain, MF_STRING, mExit, L"Изход");
    
            AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
            AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
            AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
            AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

            SetMenu(hwnd, bBar);
            InitGame();
        }
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
        if (MessageBox(hwnd, L"Ако продължиш ще загубиш тази игра !\n\nНаистина ли излизаш ?",
            L"Изход ?", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_TIMER:
        if (pause)break;
        seconds--;
        minutes = seconds / 60;
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(80, 80, 80)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }

            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1Rct.left && cur_pos.x <= b1Rct.right)
                {
                    if (!set_name && sound && !b1_hglt)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    if (!set_name && !b1_hglt)b1_hglt = true;
                }
                else
                {
                    if (b1_hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1_hglt = false;
                    }
                }
                
                if (cur_pos.x >= b2Rct.left && cur_pos.x <= b2Rct.right)
                {
                    if (!b2_hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b2_hglt = true;
                    }
                    
                }
                else if(b2_hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b2_hglt = false;
                }

                if (cur_pos.x >= b3Rct.left && cur_pos.x <= b3Rct.right)
                {
                    if (!b3_hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b3_hglt = true;
                    }
                    
                }
                else if (b3_hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b3_hglt = false;
                }

                SetCursor(outCursor);
                return true;
            }
            else
            {
                if (b1_hglt)
                {
                    b1_hglt = false;
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                }

                if (b2_hglt)
                {
                    b2_hglt = false;
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                }

                if (b3_hglt)
                {
                    b3_hglt = false;
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                }
            }

            SetCursor(mainCursor);
            return true;
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }

            if (b1_hglt)
            {
                b1_hglt = false;
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
            }

            if (b2_hglt)
            {
                b2_hglt = false;
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
            }

            if (b3_hglt)
            {
                b3_hglt = false;
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
            }

            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
            if (MessageBox(hwnd, L"Ако продължиш ще загубиш тази игра !\n\nНаистина ли рестартираш ?",
                L"Рестарт ?", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;



        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;


        }
        break;

    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)FALSE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)ErrExit(eClass);

    int file_exist = 0;
    CheckFile(LtmpFile, &file_exist);
    if (file_exist == FILE_EXIST)ErrExit(eStarted);
    else
    {
        std::wofstream start(LtmpFile);
        start << L"Rabotim, ne pipai !";
        start.close();
    }

    if (GetSystemMetrics(SM_CXSCREEN) + 50 < scr_width || GetSystemMetrics(SM_CYSCREEN) < scr_height)ErrExit(eScreen);

    mainIcon = (HICON)LoadImage(NULL, L".\\res\\bmain.ico", IMAGE_ICON, 256, 137, LR_LOADFROMFILE);
    if (!mainIcon)ErrExit(eIcon);

    mainCursor = LoadCursorFromFile(L".\\res\\bcursor.ani");
    outCursor = LoadCursorFromFile(L".\\res\\out.ani");

    bWin.hInstance = bIns;
    bWin.lpszClassName = bWinClassName;
    bWin.lpfnWndProc = &bWinProc;
    bWin.hbrBackground = CreateSolidBrush(RGB(80, 80, 80));
    bWin.hIcon = mainIcon;
    bWin.hCursor = mainCursor;
    bWin.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWin))ErrExit(eClass);

    bHwnd = CreateWindowW(bWinClassName, L"СЕКРЕТНА МИСИЯ", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 50, 5, (int)(scr_width), 
        (int)(scr_height), NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else
    {
        ShowWindow(bHwnd, SW_SHOWDEFAULT);
        InitD2D1();
    }

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessage(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkBlue));
            if (field_txt_brush && bigTextForm)
                Draw->DrawTextW(L"ПАУЗА", 6, bigTextForm, D2D1::RectF(400.0f, 300.0f, cl_width, cl_height), field_txt_brush);
            Draw->EndDraw();
            continue;
        }



        //DRAW THINGS ***********************************************************************

        Draw->BeginDraw();
        if (Field)
            Draw->DrawBitmap(bmpField[Field->GetFrame()], D2D1::RectF(0, 0, scr_width, scr_height));
        if (nrmTextForm && but_bck_brush && but_hglt_brush && but_inact_brush && but_txt_brush)
        {
            Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), but_bck_brush);
            if (set_name)
                Draw->DrawTextW(L"Име на играч", 13, nrmTextForm, D2D1::RectF(b1Rct.left + 50.0f, 0, b1Rct.right, 50.0f),
                    but_inact_brush);
            else
            {
                if(b1_hglt)
                    Draw->DrawTextW(L"Име на играч", 13, nrmTextForm, D2D1::RectF(b1Rct.left + 50.0f, 0, b1Rct.right, 50.0f),
                        but_hglt_brush);
                else
                    Draw->DrawTextW(L"Име на играч", 13, nrmTextForm, D2D1::RectF(b1Rct.left + 50.0f, 0, b1Rct.right, 50.0f),
                        but_txt_brush);
            }

            if (b2_hglt)
                Draw->DrawTextW(L"Звуци ON / OFF", 15, nrmTextForm, D2D1::RectF(b2Rct.left + 50.0f, 0, b2Rct.right, 50.0f),
                    but_hglt_brush);
            else
                Draw->DrawTextW(L"Звуци ON / OFF", 15, nrmTextForm, D2D1::RectF(b2Rct.left + 50.0f, 0, b2Rct.right, 50.0f),
                    but_txt_brush);

            if (b3_hglt)
                Draw->DrawTextW(L"Помощ за играта", 16, nrmTextForm, D2D1::RectF(b3Rct.left + 50.0f, 0, b3Rct.right, 50.0f),
                    but_hglt_brush);
            else
                Draw->DrawTextW(L"Помощ за играта", 16, nrmTextForm, D2D1::RectF(b3Rct.left + 50.0f, 0, b3Rct.right, 50.0f),
                    but_txt_brush);
        }
        


        Draw->EndDraw();


    }







    GarbageCollector();
    std::remove(tmpFile);
    return (int) bMsg.wParam;
}