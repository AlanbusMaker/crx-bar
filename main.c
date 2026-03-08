#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define MAX_LINE 256

FILE *out;
int line_number = 0;

void error(int col, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stderr,"crx-bar: error: line %d col %d: ",line_number,col);
    vfprintf(stderr,fmt,args);
    fprintf(stderr,"\n");

    va_end(args);
    exit(1);
}

void warning(int col,const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stderr,"crx-bar: warning: line %d col %d: ",line_number,col);
    vfprintf(stderr,fmt,args);
    fprintf(stderr,"\n");

    va_end(args);
}

void emit(unsigned char b) {
    fputc(b,out);
}

void emit16(unsigned short v) {
    fputc(v & 0xFF,out);
    fputc((v>>8) & 0xFF,out);
}

int find_col(char *line,char *token) {
    char *p=strstr(line,token);
    if(!p) return 1;
    return (p-line)+1;
}

void parse_line(char *line) {

    char cmd[32];

    if(sscanf(line,"%31s",cmd)!=1)
        return;

    int col=find_col(line,cmd);

    if(strcmp(cmd,"nada")==0) {
        emit(0x00);
    }

    else if(strcmp(cmd,"mov")==0) {

        int v;
        if(sscanf(line,"%*s %d",&v)!=1)
            error(col,"mov requires a value");

        emit(0x01);
        emit(v);
    }

    else if(strcmp(cmd,"add")==0) {

        int v;
        if(sscanf(line,"%*s %d",&v)!=1)
            error(col,"add requires a value");

        emit(0x02);
        emit(v);
    }

    else if(strcmp(cmd,"sub")==0) {

        int v;
        if(sscanf(line,"%*s %d",&v)!=1)
            error(col,"sub requires a value");

        emit(0x03);
        emit(v);
    }

    else if(strcmp(cmd,"mul")==0) {

        int v;
        if(sscanf(line,"%*s %d",&v)!=1)
            error(col,"mul requires a value");

        emit(0x04);
        emit(v);
    }

    else if(strcmp(cmd,"div")==0) {

        int v;
        if(sscanf(line,"%*s %d",&v)!=1)
            error(col,"div requires a value");

        emit(0x05);
        emit(v);
    }

    else if(strcmp(cmd,"int")==0) {

        int v;
        if(sscanf(line,"%*s %d",&v)!=1)
            error(col,"int requires a value");

        emit(0x06);
        emit(v);
    }

    else if(strcmp(cmd,"acp")==0) {
        emit(0x07);
    }

    else if(strcmp(cmd,"db")==0) {

        int v;
        if(sscanf(line,"%*s %d",&v)!=1)
            error(col,"db requires a value");

        emit(0x08);
        emit(v);
    }

    else if(strcmp(cmd,"dw")==0) {

        int v;
        if(sscanf(line,"%*s %d",&v)!=1)
            error(col,"dw requires a value");

        emit(0x09);
        emit16(v);
    }

    else if(strcmp(cmd,"org")==0) {

        int v;
        if(sscanf(line,"%*s %d",&v)!=1)
            error(col,"org requires value");

        /* apenas interpretado */
    }

    else if(strcmp(cmd,"bits")==0) {

        int v;
        if(sscanf(line,"%*s %d",&v)!=1)
            error(col,"bits requires value");

        if(v!=16 && v!=32 && v!=64)
            warning(col,"unusual bits value %d",v);
    }

    else if(strcmp(cmd,"times")==0) {

        int n,val;

        if(sscanf(line,"%*s %d db %d",&n,&val)!=2)
            error(col,"invalid times syntax");

        for(int i=0;i<n;i++)
            emit(val);
    }

    else {

        warning(col,"unknown instruction '%s'",cmd);

    }

}

int main(int argc,char **argv) {

    if(argc!=4 || strcmp(argv[2],"-o")!=0) {
        fprintf(stderr,"crx-bar: error: usage: crx-bar <file>.bsm -o <output>.bin\n");
        return 1;
    }

    FILE *in=fopen(argv[1],"r");

    if(!in) {
        fprintf(stderr,"crx-bar: error: cannot open '%s'\n",argv[1]);
        return 1;
    }

    out=fopen(argv[3],"wb");

    if(!out) {
        fprintf(stderr,"crx-bar: error: cannot create '%s'\n",argv[3]);
        return 1;
    }

    char line[MAX_LINE];

    while(fgets(line,sizeof(line),in)) {
        line_number++;
        parse_line(line);
    }

    fclose(in);
    fclose(out);

    return 0;
}
