/*B-em 0.6 by Tom Walker*/
/*GUI*/

#include <stdio.h>
#include <allegro.h>

char uefname[260]="test.uef";
int uefena=0;
int soundon;
int curwave;
int blurred,mono;
int ddnoise;
unsigned char *ram;
int model;
char discname[2][260];
int quit;

void save_config()
{
        FILE *f=fopen("b-em.cfg","wb");
        fputc(model,f);
        fputc(soundon,f);
        fputc(curwave,f);
        fputc(ddnoise,f);
        fputc(blurred,f);
        fputc(mono,f);
        fputc(uefena,f);
        fwrite(discname[0],260,1,f);
        fwrite(discname[1],260,1,f);
        fwrite(uefname,260,1,f);
        fclose(f);
}

void load_config()
{
        FILE *f=fopen("b-em.cfg","rb");
        model=getc(f);
        soundon=getc(f);
        curwave=getc(f);
        ddnoise=getc(f);
        blurred=getc(f);
        mono=getc(f);
        uefena=getc(f);
        fread(discname[0],260,1,f);
        fread(discname[1],260,1,f);
        fread(uefname,260,1,f);
        fclose(f);
}

int gui_exit()
{
        quit=1;
        return D_CLOSE;
}

int gui_return()
{
        return D_CLOSE;
}

MENU filemenu[]=
{
        {"Return",gui_return,NULL,NULL,NULL},
        {"Exit",gui_exit,NULL,NULL,NULL},
        {NULL,NULL,NULL,NULL,NULL}
};

int gui_changedisc0()
{
        char tempname[260];
        int ret,c;
        memcpy(tempname,discname[0],260);
        ret=file_select_ex("Please choose a disc image",tempname,"SSD;DSD;IMG",384,192);
        if (ret)
        {
                memcpy(discname[0],tempname,260);
                for (c=0;c<strlen(discname[0]);c++)
                {
                        if (discname[0][c]=='.')
                        {
                                c++;
                                break;
                        }
                }
                if ((discname[0][c]=='d'||discname[0][c]=='D')&&(c!=strlen(discname[0])))
                   load8271dsd(discname[0],0);
                else if (c!=strlen(discname[0]))
                   load8271ssd(discname[0],0);
        }
        return D_EXIT;
}

int gui_changedisc1()
{
        char tempname[260];
        int ret,c;
        memcpy(tempname,discname[1],260);
        ret=file_select_ex("Please choose a disc image",tempname,"SSD;DSD;IMG",384,192);
        if (ret)
        {
                memcpy(discname[1],tempname,260);
                for (c=0;c<strlen(discname[1]);c++)
                {
                        if (discname[1][c]=='.')
                        {
                                c++;
                                break;
                        }
                }
                if ((discname[1][c]=='d'||discname[1][c]=='D')&&(c!=strlen(discname[1])))
                   load8271dsd(discname[1],1);
                else if (c!=strlen(discname[1]))
                   load8271ssd(discname[1],1);
        }
        return D_EXIT;
}

MENU discmenu[4];

int ddsounds()
{
        ddnoise^=1;
        if (ddnoise) discmenu[2].flags=D_SELECTED;
        else         discmenu[2].flags=0;
        return D_EXIT;
}

MENU discmenu[]=
{
        {"Load disc 1",gui_changedisc0,NULL,NULL,NULL},
        {"Load disc 2",gui_changedisc1,NULL,NULL,NULL},
        {"Disc sounds",ddsounds,NULL,NULL,NULL},
        {NULL,NULL,NULL,NULL,NULL}
};

MENU tapemenu[4];

int changetape()
{
        char tempname[260];
        int ret;
        memcpy(tempname,uefname,260);
        ret=file_select_ex("Please choose a tape image",tempname,"UEF",384,192);
        if (ret)
        {
                memcpy(uefname,tempname,260);
                openuef(uefname);
        }
        return D_EXIT;
}

int rewnd=0;

int rewindtape()
{
        rewnd=1;
        return D_EXIT;
}

int tapeena()
{
        uefena^=1;
        if (uefena) tapemenu[2].flags=D_SELECTED;
        else        tapemenu[2].flags=0;
        if (uefena) loadroms();
        else        trapos();
        return D_EXIT;
}

MENU tapemenu[]=
{
        {"Change tape",changetape,NULL,NULL,NULL},
        {"Rewind tape",rewindtape,NULL,NULL,NULL},
        {"Tape enable",tapeena,NULL,NULL,NULL},
        {NULL,NULL,NULL,NULL,NULL}
};

MENU modelmenu[4];

int mpalb()
{
        model=0;
        loadroms();
        reset6502();
        reset8271(0);
        reset1770();
        resetsysvia();
        resetuservia();
        memset(ram,0,32768);
        modelmenu[0].flags=D_SELECTED;
        modelmenu[1].flags=modelmenu[2].flags=0;
        return D_EXIT;
}

int mntscb()
{
        model=1;
        loadroms();
        reset6502();
        reset8271(0);
        reset1770();
        resetsysvia();
        resetuservia();
        memset(ram,0,32768);
        modelmenu[1].flags=D_SELECTED;
        modelmenu[0].flags=modelmenu[2].flags=0;
        return D_EXIT;
}

int mpalbp()
{
        model=2;
        loadroms();
        reset6502();
        reset8271(0);
        reset1770();
        resetsysvia();
        resetuservia();
        memset(ram,0,32768);
        modelmenu[2].flags=D_SELECTED;
        modelmenu[0].flags=modelmenu[1].flags=0;
        return D_EXIT;
}

MENU modelmenu[]=
{
        {"PAL B",mpalb,NULL,NULL,NULL},
        {"NTSC B",mntscb,NULL,NULL,NULL},
        {"PAL B+",mpalbp,NULL,NULL,NULL},
        {NULL,NULL,NULL,NULL,NULL}
};

MENU videomenu[3];

int blurfilter()
{
        blurred^=1;
        if (blurred) videomenu[0].flags=D_SELECTED;
        else         videomenu[0].flags=0;
        return D_EXIT;
}

int monochrome()
{
        mono^=1;
        if (mono) videomenu[1].flags=D_SELECTED;
        else      videomenu[1].flags=0;
        updatepalette();
        return D_EXIT;
}

MENU videomenu[]=
{
        {"Blur filter",blurfilter,NULL,NULL,NULL},
        {"Monochrome",monochrome,NULL,NULL,NULL},
        {NULL,NULL,NULL,NULL,NULL}
};

MENU wavemenu[5];

int msquare()
{
        wavemenu[0].flags=D_SELECTED;
        wavemenu[1].flags=wavemenu[2].flags=wavemenu[3].flags=0;
        curwave=0;
        return D_EXIT;
}

int msaw()
{
        wavemenu[1].flags=D_SELECTED;
        wavemenu[0].flags=wavemenu[2].flags=wavemenu[3].flags=0;
        curwave=1;
        return D_EXIT;
}

int msine()
{
        wavemenu[2].flags=D_SELECTED;
        wavemenu[1].flags=wavemenu[0].flags=wavemenu[3].flags=0;
        curwave=2;
        return D_EXIT;
}

int mtri()
{
        wavemenu[3].flags=D_SELECTED;
        wavemenu[1].flags=wavemenu[2].flags=wavemenu[0].flags=0;
        curwave=3;
        return D_EXIT;
}

MENU wavemenu[]=
{
        {"Square",msquare,NULL,NULL,NULL},
        {"Sawtooth",msaw,NULL,NULL,NULL},
        {"Sine",msine,NULL,NULL,NULL},
        {"Triangle",mtri,NULL,NULL,NULL},
        {NULL,NULL,NULL,NULL,NULL}
};

MENU soundmenu[5];

int soundena()
{
        soundon^=1;
        if (soundon) soundmenu[0].flags=D_SELECTED;
        else         soundmenu[0].flags=0;
        if (!soundon) set_volume(0,0);
        else          set_volume(255,0);
        return D_EXIT;
}

int startlog()
{
        char tempname[260]="";
        int ret;
        ret=file_select_ex("Please enter a file name",tempname,"SN",384,192);
        if (ret)
           startsnlog(tempname);
        return D_EXIT;
}

int stoplog()
{
        stopsnlog();
        return D_EXIT;
}

MENU soundmenu[]=
{
        {"Sound enable",soundena,NULL,NULL,NULL},
        {"Waveform",NULL,wavemenu,NULL,NULL},
        {"Start SN log",startlog,NULL,NULL,NULL},
        {"Stop SN log",stoplog,NULL,NULL,NULL},
        {NULL,NULL,NULL,NULL,NULL}
};

int calib1()
{
        char *msg;
        char s[128];
        while (joy[0].flags&JOYFLAG_CALIBRATE)
        {
                msg=calibrate_joystick_name(0);
                sprintf(s,"%s, and press OK\n",msg);
                alert(NULL,s,NULL,"OK",NULL,0,0);
                if (calibrate_joystick(0))
                {
                        alert(NULL,"Calibration error",NULL,"OK",NULL,0,0);
                        return D_EXIT;
                }
        }
        return D_EXIT;
}

int calib2()
{
        char *msg;
        char s[128];
        while (joy[1].flags&JOYFLAG_CALIBRATE)
        {
                msg=calibrate_joystick_name(1);
                sprintf(s,"%s, and press OK\n",msg);
                alert(NULL,s,NULL,"OK",NULL,0,0);
                if (calibrate_joystick(1))
                {
                        alert(NULL,"Calibration error",NULL,"OK",NULL,0,0);
                        return D_EXIT;
                }
        }
        return D_EXIT;
}

int mscrshot()
{
        char fn[260];
        if (file_select_ex("Enter a file name",fn,"BMP;PCX;TGA;LBM",384,192))
        {
                restorepal();
                scrshot(fn);
                fadepal();
        }
        return D_EXIT;
}

MENU joymenu[]=
{
        {"Calibrate joystick 1",calib1,NULL,NULL,NULL},
        {"Calibrate joystick 2",calib2,NULL,NULL,NULL},
        {"Save screenshot",mscrshot,NULL,NULL,NULL},
        {NULL,NULL,NULL,NULL,NULL}
};

MENU mainmenu[]=
{
        {"File",NULL,filemenu,NULL,NULL},
        {"Model",NULL,modelmenu,NULL,NULL},
        {"Disc",NULL,discmenu,NULL,NULL},
        {"Tape",NULL,tapemenu,NULL,NULL},
        {"Video",NULL,videomenu,NULL,NULL},
        {"Sound",NULL,soundmenu,NULL,NULL},
        {"Misc",NULL,joymenu,NULL,NULL},
        {NULL,NULL,NULL,NULL,NULL}
};

DIALOG bemgui[]=
{
      {d_ctext_proc, 200, 260, 0,  0, 15,0,0,0,     0,0,"B-em v0.6"},
      {d_menu_proc,  0,   0,   0,  0, 15,0,0,0,     0,0,mainmenu},
      {0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL}
};

BITMAP *mouse,*_mouse_sprite;

void entergui()
{
        int x,y;
        fadepal();
        if (uefena) tapemenu[2].flags=D_SELECTED;
        else        tapemenu[2].flags=0;
        if (blurred) videomenu[0].flags=D_SELECTED;
        else         videomenu[0].flags=0;
        if (mono) videomenu[1].flags=D_SELECTED;
        else      videomenu[1].flags=0;
        if (ddnoise) discmenu[2].flags=D_SELECTED;
        else         discmenu[2].flags=0;
        if (soundon) soundmenu[0].flags=D_SELECTED;
        else         soundmenu[0].flags=0;
        modelmenu[0].flags=modelmenu[1].flags=modelmenu[2].flags=0;
        if (model==0) modelmenu[0].flags=D_SELECTED;
        if (model==1) modelmenu[1].flags=D_SELECTED;
        if (model==2) modelmenu[2].flags=D_SELECTED;
        wavemenu[0].flags=wavemenu[1].flags=wavemenu[2].flags=wavemenu[3].flags=0;
        wavemenu[curwave].flags=D_SELECTED;
        if (!mouse)
        {
                mouse=create_bitmap(10,16);
                for (y=0;y<16;y++)
                {
                        for (x=0;x<10;x++)
                        {
                                switch (getpixel(_mouse_sprite,x,y))
                                {
                                        case 0:
                                        putpixel(mouse,x,y,0);
                                        break;
                                        case 16:
                                        putpixel(mouse,x,y,15);
                                        break;
                                        case 255:
                                        putpixel(mouse,x,y,0);
                                        break;
                                }
                        }
                }
                set_mouse_sprite(mouse);
        }
        gui_fg_color=15;
        clear_keybuf();
        set_volume(0,0);
        do_dialog(bemgui,0);
        set_volume(255,0);
//        do_menu(mainmenu,0,0);
        clear_keybuf();
        restorepal();
        if (rewnd)
        {
                rewindit();
                rewnd=0;
        }
}
