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

dirs ship_prev_dir = dirs::stop;

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
ID2D1Bitmap* bmpSpare = nullptr;

ID2D1Bitmap* bmpExplosion[24] = { 0 };
ID2D1Bitmap* bmpField[10] = { 0 };
ID2D1Bitmap* bmpShipL[13] = { 0 };
ID2D1Bitmap* bmpShipR[13] = { 0 };

//////////////////////////////////////

prot_ptr Field = nullptr;
prot_ptr Ship = nullptr;
std::vector<prot_ptr>vRocks;
std::vector<prot_ptr>vLasers;
std::vector<prot_ptr>vBadLasers;

prot_ptr BadShip = nullptr;
prot_ptr Cloud1 = nullptr;
prot_ptr Cloud2 = nullptr;
bas_ptr Portal = nullptr;
bas_ptr Spare = nullptr;

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
    if (err)ErrLog(L"Error in RelaseCOM(&Draw)");

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

    for (int i = 0; i < 24; i++)
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

    for (int i = 0; i < 10; i++)
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
    wcscpy_s(current_player, L"A SPY");
    set_name = false;

    if (!vRocks.empty())
    {
        for (int i = 0; i < vRocks.size(); ++i)ReleaseCOM(&vRocks[i]);
    }

    if (!vLasers.empty())
    {
        for (int i = 0; i < vLasers.size(); ++i)ReleaseCOM(&vLasers[i]);
    }

    if (!vBadLasers.empty())
    {
        for (int i = 0; i < vBadLasers.size(); ++i)ReleaseCOM(&vBadLasers[i]);
    }

    ReleaseCOM(&Field);
    ReleaseCOM(&Ship);
    ReleaseCOM(&BadShip);
    ReleaseCOM(&Cloud1);
    ReleaseCOM(&Cloud2);
    ReleaseCOM(&Portal);
    ReleaseCOM(&Spare);
    vRocks.clear();
    vLasers.clear();
    vBadLasers.clear();

    Field = ProtonFactory(types::field, 0, 0);
    Ship = ProtonFactory(types::ship, 120.0f, cl_height / 2);
 
    for (int i = 0; i < 8; i++)
    {
        types one_type = types::no_type;
        float start_x = 0;
        float start_y = 0;

        switch (rand() % 4)
        {
        case 0:
            one_type = types::big_block_u;
            break;

        case 1:
            one_type = types::big_block_d;
            break;

        case 2:
            one_type = types::small_block;
            break;

        case 3:
            one_type = types::vblock;
            break;
        }

        if (i == 0)
        {
            start_x = cl_width;
            if (one_type == types::big_block_u)start_y = 51.0f;
            else if (one_type == types::big_block_d)start_y = 550.0f;
            else if (one_type == types::small_block)start_y = 51.0f;
            else if (one_type == types::vblock) start_y = 51.0f + rand() % 200;
        }
        else
        {
            start_x = vRocks.back()->ex;
            if (one_type == types::big_block_u)start_y = 51.0f;
            else if (one_type == types::big_block_d)start_y = 550.0f;
            else if (one_type == types::small_block)start_y = 51.0f;
            else if (one_type == types::vblock) start_y = 51.0f + rand() % 200;
        }
        vRocks.push_back(ProtonFactory(one_type, start_x, start_y));
    }

}
BOOL CheckRecord()
{
    if (score < 1)return no_record;

    int result = 0;
    CheckFile(recordFile, &result);
    if (result == FILE_NOT_EXIST)
    {
        std::wofstream rec(recordFile);
        rec << score << std::endl;
        for (int i = 0; i < 16; i++)rec << static_cast<int>(current_player[i]) << std::endl;
        rec.close();
        return first_record;
    }

    std::wifstream check(recordFile);
    check >> result;
    check.close();

    if (result < score)
    {
        std::wofstream rec(recordFile);
        rec << score << std::endl;
        for (int i = 0; i < 16; i++)rec << static_cast<int>(current_player[i]) << std::endl;
        rec.close();
        return record;
    }
    return no_record;
}
void GameOver()
{
    KillTimer(bHwnd, bTimer);
    PlaySound(NULL, NULL, NULL);

    wchar_t final_text[20] = L"О, О, О ! ЗАГУБИ !";
    wchar_t show_text[20] = L"\0";

    switch (CheckRecord())
    {
    case no_record:
        if (sound)PlaySound(L".\\res\\snd\\loose.wav", NULL, SND_ASYNC);
        break;

    case first_record:
        if (sound)PlaySound(L".\\res\\snd\\record.wav", NULL, SND_ASYNC);
        wcscpy_s(final_text, L"ПЪРВИ РЕКОРД НА ИГРАТА !");
        break;

    case record:
        if (sound)PlaySound(L".\\res\\snd\\record.wav", NULL, SND_ASYNC);
        wcscpy_s(final_text, L"СВЕТОВЕН РЕКОРД НА ИГРАТА !");
        break;
    }

    for (int i = 0; i < 20; i++)
    {
        mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
        show_text[i] = final_text[i];
        if (Draw && bigTextForm && field_txt_brush)
        {
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkBlue));
            Draw->DrawTextW(show_text, i, bigTextForm, D2D1::RectF(200.0f, 300.0f, cl_width, cl_height), field_txt_brush);
            Draw->EndDraw();
            Sleep(30);
        }
    }
    Sleep(2500);

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

    bmpCloud2 = Load(L".\\res\\img\\cloud2.png", Draw);
    if (!bmpCloud2)ErrExit(eD2D, L"Error loading bmpCloud2");

    bmpGreatBlock = Load(L".\\res\\img\\vblock.png", Draw);
    if (!bmpGreatBlock)ErrExit(eD2D, L"Error loading bmpGreatBlock");

    bmpPortal = Load(L".\\res\\img\\portal.png", Draw);
    if (!bmpPortal)ErrExit(eD2D, L"Error loading bmpPortal");

    bmpSmallBlock = Load(L".\\res\\img\\small_block.png", Draw);
    if (!bmpSmallBlock)ErrExit(eD2D, L"Error loading bmpSmallBlock");

    bmpSpare = Load(L".\\res\\img\\spare.png", Draw);
    if (!bmpSpare)ErrExit(eD2D, L"Error loading bmpSpare");

    bmpLaser = Load(L".\\res\\img\\laser.png", Draw);
    if (!bmpLaser)ErrExit(eD2D, L"Error loading bmpLaser");

    for (int i = 0; i < 24; i++)
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
void NewLevel()
{
    if (sound)mciSendString(L"play .\\res\\snd\\levelup.wav", NULL, NULL, NULL);
    wchar_t start_text[19] = L"НИВОТО ПРЕМИНАТО !";
    wchar_t show_text[19] = L"\0";

    for (int i = 0; i < 19; i++)
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


    int ship_current_lifes = 0;
    if (Ship)ship_current_lifes = Ship->lifes;

    seconds = 180 + level * 10;
    level++;

    if (!vRocks.empty())
    {
        for (int i = 0; i < vRocks.size(); ++i)ReleaseCOM(&vRocks[i]);
    }

    if (!vLasers.empty())
    {
        for (int i = 0; i < vLasers.size(); ++i)ReleaseCOM(&vLasers[i]);
    }

    if (!vBadLasers.empty())
    {
        for (int i = 0; i < vBadLasers.size(); ++i)ReleaseCOM(&vBadLasers[i]);
    }

    ReleaseCOM(&Field);
    ReleaseCOM(&Ship);
    ReleaseCOM(&BadShip);
    ReleaseCOM(&Cloud1);
    ReleaseCOM(&Cloud2);
    ReleaseCOM(&Portal);
    ReleaseCOM(&Spare);
    vRocks.clear();
    vLasers.clear();
    vBadLasers.clear();

    Field = ProtonFactory(types::field, 0, 0);
    Ship = ProtonFactory(types::ship, 120.0f, cl_height / 2);
    Ship->lifes = ship_current_lifes; 

    for (int i = 0; i < 8; i++)
    {
        types one_type = types::no_type;
        float start_x = 0;
        float start_y = 0;

        switch (rand() % 4)
        {
        case 0:
            one_type = types::big_block_u;
            break;

        case 1:
            one_type = types::big_block_d;
            break;

        case 2:
            one_type = types::small_block;
            break;

        case 3:
            one_type = types::vblock;
            break;
        }

        if (i == 0)
        {
            start_x = cl_width;
            if (one_type == types::big_block_u)start_y = 51.0f;
            else if (one_type == types::big_block_d)start_y = 550.0f;
            else if (one_type == types::small_block)start_y = 51.0f;
            else if (one_type == types::vblock) start_y = 51.0f + rand() % 200;
        }
        else
        {
            start_x = vRocks.back()->ex;
            if (one_type == types::big_block_u)start_y = 51.0f;
            else if (one_type == types::big_block_d)start_y = 550.0f;
            else if (one_type == types::small_block)start_y = 51.0f;
            else if (one_type == types::vblock) start_y = 51.0f + rand() % 200;
        }
        vRocks.push_back(ProtonFactory(one_type, start_x, start_y));
    }

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
        if (seconds <= 0)
        {
            if (!Portal)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\warp.wav", NULL, NULL, NULL);
                Portal = new BASIC(850.0f, 51.0f + rand() % 550, 100.0f, 100.0f);
            }
            break;
        }
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

        case mSpeed:
            pause = true;
            if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
            if (MessageBox(hwnd, L"Ако продължиш ще загубиш бонусите за нивото !\n\nНаистина ли минаваш на следващо ниво ?",
                L"Ново ниво ?", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            NewLevel();
            break;



        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;


        }
        break;

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case VK_LEFT:
            if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
            if (Ship)Ship->dir = dirs::left;
            break;

        case VK_RIGHT:
            if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
            if (Ship)Ship->dir = dirs::right;
            break;

        case VK_UP:
            if (Ship)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
                if (Ship->dir != dirs::up && Ship->dir != dirs::down)ship_prev_dir = Ship->dir;
                Ship->dir = dirs::up;
            }
            break;

        case VK_DOWN:
            if (Ship)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
                if (Ship->dir != dirs::up && Ship->dir != dirs::down)ship_prev_dir = Ship->dir; 
                Ship->dir = dirs::down;
            }
            break;

        case VK_CONTROL:
            if (sound)mciSendString(L"play .\\res\\snd\\laser.wav", NULL, NULL, NULL);
            if (Ship)
                vLasers.push_back(ProtonFactory(types::laser, Ship->ex, Ship->y + 20.0f));
            break;

        }
        break;

    case WM_LBUTTONDOWN:
        if (LOWORD(lParam) >= b1Rct.left && LOWORD(lParam) <= b1Rct.right && HIWORD(lParam) <= 50.0f)
        {
            if (set_name)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                break;
            }
            if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
            if (DialogBox(bIns, MAKEINTRESOURCE(IDD_PLAYER), hwnd, &bDlgProc) == IDOK)set_name = true;
            break;
        }
        if (LOWORD(lParam) >= b2Rct.left && LOWORD(lParam) <= b2Rct.right && HIWORD(lParam) <= 50.0f)
        {
            if (sound)
            {
                mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                sound = false;
                PlaySound(NULL, NULL, NULL);
                break;
            }
            else
            {
                sound = true;
                PlaySound(soundFile, NULL, SND_ASYNC | SND_LOOP);
                break;
            }
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

        //GAME ENGINE *********************************************************************

        //SHIP *******************

        if (Ship)
        {
            if (Ship->GetType() != types::explosion)Ship->Move((float)(level));
        }
        if (!vRocks.empty())
        {
            for (std::vector<prot_ptr>::iterator rock = vRocks.begin(); rock < vRocks.end(); ++rock)
            {
                if ((*rock)->Move((float)(level)) == DLRESULT::fail)
                {
                    (*rock)->Release();
                    vRocks.erase(rock);
                    break;
                }
            }
        }
        if (vRocks.size() < 7)
        {
            types one_type = types::no_type;
            float start_x = vRocks.back()->ex;
            float start_y = 0;

            switch (rand() % 4)
            {
            case 0:
                one_type = types::big_block_u;
                break;

            case 1:
                one_type = types::big_block_d;
                break;

            case 2:
                one_type = types::small_block;
                break;

            case 3:
                one_type = types::vblock;
                break;
            }
            if (one_type == types::big_block_u)start_y = 51.0f;
            else if (one_type == types::big_block_d)start_y = 550.0f;
            else if (one_type == types::small_block)start_y = 51.0f;
            else if (one_type == types::vblock)start_y = 51.0f + rand() % 200;

            vRocks.push_back(ProtonFactory(one_type, start_x, start_y));
            
        }
        if (Ship && !vRocks.empty())
        {
            if (Ship->GetType() != types::explosion)
            {
                for (int i = 0; i < vRocks.size(); i++)
                {
                    if (!(Ship->x > vRocks[i]->ex || Ship->ex < vRocks[i]->x || Ship->y > vRocks[i]->ey || Ship->ey < vRocks[i]->y))
                    {
                        Ship->Transform(types::explosion);
                        break;
                    }
                }
            }
        }
        if (Ship && BadShip)
        {
            if (Ship->GetType() != types::explosion && BadShip->GetType() != types::explosion)
            {
                if (!(Ship->x > BadShip->ex || Ship->ex < BadShip->x || Ship->y > BadShip->ey || Ship->ey < BadShip->y))
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                    Ship->Transform(types::explosion);
                }
            }
        }
        if (Ship && Spare)
        {
            if (!(Ship->x >= Spare->ex || Ship->ex <= Spare->x || Ship->y >= Spare->ey || Ship->ey <= Spare->y))
            {
                if (sound)mciSendString(L"play .\\res\\snd\\lifes.wav", NULL, NULL, NULL);
                if (Ship->lifes < 120)
                {
                    Ship->lifes += 30;
                    if (Ship->lifes > 120)Ship->lifes = 120;
                }
                else score += 30;
                ReleaseCOM(&Spare);
                Spare = nullptr;
            }
        }

        /////////////////////////

        // BAD SHIP ****************************

        //MOVE ****************
        if (!BadShip)
        {
            if (rand() % 100 == 33)BadShip = ProtonFactory(types::battleship, cl_width + 100.0f, 55.0f + 
                static_cast<float>(rand() % 450));
        }
        else if (BadShip->GetType() != types::explosion)
        {
            bas_ptr dummy = nullptr;
            float dummy_speed = 0.8f * level;
            bool left_ok = true;
            bool up_ok = true;
            bool down_ok = true;

            if (BadShip->dir == dirs::left)
            {
                dummy = new BASIC(BadShip->x, BadShip->y, 100.0f, 51.0f);

                dummy->x -= dummy_speed;
                dummy->SetEdges();

                if (!vRocks.empty())
                {
                    for (std::vector<prot_ptr>::iterator it = vRocks.begin(); it < vRocks.end(); it++)
                    {
                        if (!(dummy->x >= (*it)->ex || dummy->ex <= (*it)->x
                            || dummy->y >= (*it)->ey || dummy->ey <= (*it)->y))
                        {
                            left_ok = false;
                            break;
                        }
                    }
                }
            }

            if (BadShip->dir == dirs::down)
            {
                ReleaseCOM(&dummy);
                dummy = new BASIC(BadShip->x, BadShip->y, 100.0f, 51.0f);

                dummy->y += dummy_speed;
                dummy->SetEdges();

                if (!vRocks.empty())
                {
                    for (std::vector<prot_ptr>::iterator it = vRocks.begin(); it < vRocks.end(); it++)
                    {
                        if (!(dummy->x > (*it)->ex || dummy->ex < (*it)->x
                            || dummy->y >= (*it)->ey || dummy->ey <= (*it)->y))
                        {
                            down_ok = false;
                            break;
                        }
                    }
                }
            }
            
            if (BadShip->dir == dirs::up)
            {
                ReleaseCOM(&dummy);
                dummy = new BASIC(BadShip->x, BadShip->y, 100.0f, 51.0f);

                dummy->y -= dummy_speed;
                dummy->SetEdges();

                if (vRocks.empty())
                {
                    for (std::vector<prot_ptr>::iterator it = vRocks.begin(); it < vRocks.end(); it++)
                    {
                        if (!(dummy->x > (*it)->ex || dummy->ex < (*it)->x
                            || dummy->y >= (*it)->ey || dummy->ey <= (*it)->y))
                        {
                            up_ok = false;
                            break;
                        }
                    }
                }
            }

            if (left_ok) BadShip->dir = dirs::left;
            else
            {
                if (down_ok) BadShip->dir = dirs::down;
                else if (up_ok)BadShip->dir = dirs::up;
            }

            if (!left_ok && !down_ok && !up_ok) BadShip->Transform(types::explosion);

            if (BadShip->GetType() != types::explosion)
            {
                if (BadShip->Move((float)(level)) == DLRESULT::fail)
                {
                    BadShip->Release();
                    BadShip = nullptr;
                }
            }
            ReleaseCOM(&dummy);
        }

        if (BadShip && rand() % 200 == 66)
        {
            if (sound)mciSendString(L"play .\\res\\snd\\laser.wav", NULL, NULL, NULL);
            vBadLasers.push_back(ProtonFactory(types::bad_laser, BadShip->x, BadShip->y + 25.0f));
        }

        // LASERS *****************************
        if (!vBadLasers.empty())
        {
            for (std::vector<prot_ptr>::iterator las = vBadLasers.begin(); las < vBadLasers.end(); las++)
            {
                if ((*las)->Move((float)(level)) == DLRESULT::fail)
                {
                    (*las)->Release();
                    vBadLasers.erase(las);
                    break;
                }
            }
        }
        if (!vLasers.empty())
        {
            for (std::vector<prot_ptr>::iterator las = vLasers.begin(); las < vLasers.end(); las++)
            {
                if ((*las)->Move((float)(level)) == DLRESULT::fail)
                {
                    (*las)->Release();
                    vLasers.erase(las);
                    break;
                }
            }
        }

        if (!vBadLasers.empty())
        {
            for (std::vector<prot_ptr>::iterator las = vBadLasers.begin(); las < vBadLasers.end(); las++)
            {
                if (Ship)
                {
                    if (!(Ship->x >= (*las)->ex || Ship->ex <= (*las)->x || Ship->y >= (*las)->ey || Ship->ey <= (*las)->y))
                    {
                        Ship->lifes -= 30;
                        if (Ship->lifes <= 0)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                            Ship->Transform(types::explosion);
                        }
                        else if (sound)mciSendString(L"play .\\res\\snd\\damage.wav", NULL, NULL, NULL);
                        (*las)->Release();
                        vBadLasers.erase(las);
                        break;
                    }
                }
            }
        }
        if (!vLasers.empty())
        {
            for (std::vector<prot_ptr>::iterator las = vLasers.begin(); las < vLasers.end(); las++)
            {
                if (BadShip)
                {
                    if (!(BadShip->x >= (*las)->ex || BadShip->ex <= (*las)->x || 
                        BadShip->y >= (*las)->ey || BadShip->ey <= (*las)->y))
                    {
                        BadShip->lifes -= 20;
                        if (BadShip->lifes <= 0)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                            score += 50;
                            BadShip->Transform(types::explosion);
                        }
                        else if (sound)mciSendString(L"play .\\res\\snd\\damage.wav", NULL, NULL, NULL);

                        (*las)->Release();
                        vLasers.erase(las);
                        break;
                    }
                }
            }
        }

        if (!vLasers.empty() && !vRocks.empty())
        {
            for (std::vector<prot_ptr>::iterator rock = vRocks.begin(); rock < vRocks.end(); rock++)
            {
                bool killed = false;

                for (std::vector<prot_ptr>::iterator laser = vLasers.begin(); laser < vLasers.end(); laser++)
                {
                    if (!((*laser)->x >= (*rock)->ex || (*laser)->ex <= (*rock)->x
                        || (*laser)->y >= (*rock)->ey || (*laser)->ey <= (*rock)->y))
                    {
                        (*laser)->Release();
                        vLasers.erase(laser);
                        if ((*rock)->GetType() == types::vblock)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\damage.wav", NULL, NULL, NULL);
                            (*rock)->Transform(types::big_block_u);
                            score += 30;
                        }
                        killed = true;
                        break;
                    }
                }
                if (killed)break;
            }
        }


        // BUBBLES ****************************

        if (!Cloud1 && rand() % 300 == 22)Cloud1 = ProtonFactory(types::cloud1, scr_width, (float)(rand() % 500 + 55));
        if (!Cloud2 && rand() % 250 == 33)Cloud2 = ProtonFactory(types::cloud2, scr_width, (float)(rand() % 500 + 55));

        if (Cloud1)
        {
            if (rand() % 450 == 33)Cloud1->Transform(types::cloud2);
            if (Cloud1->Move((float)(level)) == DLRESULT::fail)
            {
                Cloud1->Release();
                Cloud1 = nullptr;
            }
        }

        if (Cloud2)
        {
            if (rand() % 280 == 33)Cloud2->Transform(types::cloud1);
            if (Cloud2->Move((float)(level)) == DLRESULT::fail)
            {
                Cloud2->Release();
                Cloud2 = nullptr;
            }
        }

        // PORTAL *****************************

        if (Ship && Portal)
        {
            if (!(Ship->x > Portal->ex - 20.0f || Ship->ex + 20.0f < Portal->x
                || Ship->y > Portal->ey - 20.0f || Ship->ey + 20.0f < Portal->y))NewLevel();
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
        Draw->DrawLine(D2D1::Point2F(0, 685.0f), D2D1::Point2F(scr_width, 685.0f), field_txt_brush, 10.0f);
        if (!vRocks.empty())
        {
            for (int i = 0; i < vRocks.size(); ++i)
            {
                switch (vRocks[i]->GetType())
                {
                case types::big_block_u:
                    Draw->DrawBitmap(bmpBigBlockU, D2D1::RectF(vRocks[i]->x, vRocks[i]->y, vRocks[i]->ex, vRocks[i]->ey));
                    break;

                case types::big_block_d:
                    Draw->DrawBitmap(bmpBigBlockD, D2D1::RectF(vRocks[i]->x, vRocks[i]->y, vRocks[i]->ex, vRocks[i]->ey));
                    break;

                case types::small_block:
                    Draw->DrawBitmap(bmpSmallBlock, D2D1::RectF(vRocks[i]->x, vRocks[i]->y, vRocks[i]->ex, vRocks[i]->ey));
                    break;

                case types::vblock:
                    Draw->DrawBitmap(bmpGreatBlock, D2D1::RectF(vRocks[i]->x, vRocks[i]->y, vRocks[i]->ex, vRocks[i]->ey));
                    break;
                }
            }
        }

        wchar_t status_text[200] = L"\0";
        wchar_t add[5] = L"\0";
        int stat_size = 0;

        wcscpy_s(status_text, current_player);

        wcscat_s(status_text, L", ниво: ");
        swprintf(add, 5, L"%d", level);
        wcscat_s(status_text, add);

        wcscat_s(status_text, L", резултат: ");
        swprintf(add, 5, L"%d", score);
        wcscat_s(status_text, add);

        for (int i = 0; i < 200; ++i)
        {
            if (status_text[i] != '\0')stat_size++;
            else break;
        }

        if (nrmTextForm && field_txt_brush)
            Draw->DrawText(status_text, stat_size, bigTextForm, D2D1::RectF(10.0f, 700.0f, cl_width, cl_height), field_txt_brush);

        wcscpy_s(status_text, L"0");
        swprintf(add, 3, L"%d", minutes);
        wcscat_s(status_text, add);
        wcscat_s(status_text, L" : ");
        if (seconds < 10)wcscat_s(status_text, L"0");
        swprintf(add, 3, L"%d", seconds - minutes * 60);
        if (seconds - minutes * 60 < 10)wcscat_s(status_text, L"0");
        wcscat_s(status_text, add);

        stat_size = 0;
        for (int i = 0; i < 200; ++i)
        {
            if (status_text[i] != '\0')stat_size++;
            else break;
        }

        if (nrmTextForm && field_txt_brush)
            Draw->DrawText(status_text, stat_size, bigTextForm, D2D1::RectF(450.0f, 60.0f, 650.0f, 360.0f), field_txt_brush);


        /////////////////////////////////////////////////////////////////////////////////////
        
        if (Portal)
            Draw->DrawBitmap(bmpPortal, D2D1::RectF(Portal->x, Portal->y, Portal->ex, Portal->ey));
        if (Ship)
        {
            if (Ship->GetType() != types::explosion)
            {
                ID2D1RadialGradientBrush* AuraBrush = nullptr;
                D2D1_GRADIENT_STOP oneStop[2] = { 0 };
                ID2D1GradientStopCollection* StopCollection = nullptr;

                if (Ship->lifes > 80)
                {
                    oneStop[0].position = 0;
                    oneStop[0].color = D2D1::ColorF(D2D1::ColorF::DarkGreen);
                    oneStop[1].position = 1.0f;
                    oneStop[1].color = D2D1::ColorF(D2D1::ColorF::GreenYellow);
                }
                else if (Ship->lifes <= 80 && Ship->lifes > 40)
                {
                    oneStop[0].position = 0;
                    oneStop[0].color = D2D1::ColorF(D2D1::ColorF::Yellow);
                    oneStop[1].position = 1.0f;
                    oneStop[1].color = D2D1::ColorF(D2D1::ColorF::LightYellow);
                }
                else
                {
                    oneStop[0].position = 0;
                    oneStop[0].color = D2D1::ColorF(D2D1::ColorF::DarkRed);
                    oneStop[1].position = 1.0f;
                    oneStop[1].color = D2D1::ColorF(D2D1::ColorF::OrangeRed);
                }

                if (Draw)
                {
                    Draw->CreateGradientStopCollection(oneStop, 2, &StopCollection);
                    Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(Ship->x + 50.0f,
                        Ship->y + 18.0f), D2D1::Point2F(0, 0), 70.0f, 38.0f), StopCollection, &AuraBrush);
                    if (AuraBrush)
                        Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(Ship->x + 50.0f, Ship->y + 18.0f), 70.0f, 38.0f), AuraBrush);
                }
                ReleaseCOM(&AuraBrush);
                ReleaseCOM(&StopCollection);
            }
        }
        if (Ship)
        {
            int ex_frame = Ship->GetFrame();

            if (Ship->GetType() == types::explosion)
            {
                Draw->DrawBitmap(bmpExplosion[ex_frame], D2D1::RectF(Ship->x, Ship->y - 100.0f, Ship->ex + 50.0f, Ship->ey));
                if (ex_frame >= 23)GameOver();
            }
            else
            {
                if (Ship->dir == dirs::right || Ship->dir == dirs::stop)
                    Draw->DrawBitmap(bmpShipR[ex_frame], D2D1::RectF(Ship->x, Ship->y, Ship->ex, Ship->ey));
                else if (Ship->dir == dirs::left)
                    Draw->DrawBitmap(bmpShipL[ex_frame], D2D1::RectF(Ship->x, Ship->y, Ship->ex, Ship->ey));
                else if (Ship->dir == dirs::up || Ship->dir == dirs::down)
                {
                    if (ship_prev_dir == dirs::right || ship_prev_dir == dirs::stop)
                        Draw->DrawBitmap(bmpShipR[ex_frame], D2D1::RectF(Ship->x, Ship->y, Ship->ex, Ship->ey));
                    else if (ship_prev_dir == dirs::left)
                        Draw->DrawBitmap(bmpShipL[ex_frame], D2D1::RectF(Ship->x, Ship->y, Ship->ex, Ship->ey));
                }
            }
            
        }
        
        if (BadShip)
        {
            if (BadShip->GetType() == types::explosion)
            {
                
                int ex_frame = BadShip->GetFrame();
                Draw->DrawBitmap(bmpExplosion[ex_frame], D2D1::RectF(BadShip->x, BadShip->y - 100.0f, 
                    BadShip->ex + 50.0f, BadShip->ey));
                if (ex_frame >= 23)
                {
                    if (rand() % 10 == 3 && !Spare)Spare = new BASIC(BadShip->x, BadShip->y, 50.0f, 50.0f);
                    BadShip->Release();
                    BadShip = nullptr;
                }
            }
            else
                Draw->DrawBitmap(bmpBadShip, D2D1::RectF(BadShip->x, BadShip->y, BadShip->ex, BadShip->ey));
        }
        if (!vBadLasers.empty())
            for (int i = 0; i < vBadLasers.size(); ++i)
                Draw->DrawBitmap(bmpBadLaser, D2D1::RectF(vBadLasers[i]->x, vBadLasers[i]->y,
                    vBadLasers[i]->ex, vBadLasers[i]->ey));
        if (!vLasers.empty())
            for (int i = 0; i < vLasers.size(); ++i)
                Draw->DrawBitmap(bmpLaser, D2D1::RectF(vLasers[i]->x, vLasers[i]->y,
                    vLasers[i]->ex, vLasers[i]->ey));
        if (Spare)Draw->DrawBitmap(bmpSpare, D2D1::RectF(Spare->x, Spare->y, Spare->ex, Spare->ey));
        if (Cloud1)
            Draw->DrawBitmap(bmpCloud1, D2D1::RectF(Cloud1->x, Cloud1->y, Cloud1->ex, Cloud1->ey));
        if (Cloud2)
            Draw->DrawBitmap(bmpCloud2, D2D1::RectF(Cloud2->x, Cloud2->y, Cloud2->ex, Cloud2->ey));


        //////////////////////////////////////////////
        Draw->EndDraw();
    }

    GarbageCollector();
    std::remove(tmpFile);
    return (int) bMsg.wParam;
}