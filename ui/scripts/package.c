#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef int32_t (*processs)(void *oObj, const char *args, const char *formats);

int32_t systemArgs(void *oObj, const char *cmd, 
        const char *args, processs myprocess) {
    FILE *fp        = NULL;
    char buf[2048]  = {0};

    if((fp = popen(cmd, "r")) == NULL ) {
        fprintf(stderr, "popen error!\n");
        return -1;
    }

    while (fgets(buf, sizeof(buf), fp)) {
        if (NULL != strstr(buf, args)) {
            myprocess(oObj, args, buf);
        }
    }

    pclose(fp);

    return 0;
}

int32_t getVersion(void *oObj, const char *args, 
        const char *formats) {
    int32_t index   = 0;
    char *version   = (char *)oObj;

    if (version) {
        strcpy(version, formats);

        while (version[index] != '\0') {
            if (version[index] == ' ') {
                version[index] = '\0';
                break;
            }

            index++;
        }
    }

    return 0;
}

int32_t main(int32_t args, char *argv[]) {
    int8_t formats[2048];
    int8_t version[128];

    if (args != 3) {
        fprintf (stderr, "inputs :args != 3\n");
        return -1;
    }

    fprintf (stderr, "args:%d >> argv[0]:%s argv[1]:%s argv[2]:%s\n",
            args, argv[0], argv[1], argv[2]);
    snprintf (formats, sizeof(formats) - 1, "rm -rfd %s/build/", argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "mkdir -p %s/build/", argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "mkdir -p %s/build/IPKG_BUILD/", argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "mkdir -p %s/build/IPKG_BUILD/etc", argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "mkdir -p %s/build/IPKG_BUILD/etc/init.d/", argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "mkdir -p %s/build/IPKG_BUILD/root/", argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "mkdir -p %s/build/IPKG_BUILD/root/jgd215/", argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "cp -rfd %s/scripts/S99jgd215.sh %s/build/IPKG_BUILD/etc/init.d/", argv[1], argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "cp -rfd %s/scripts/start.sh %s/build/IPKG_BUILD/root/jgd215/", argv[1], argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "cp -rfd %s/pre_build/%s/lighttpd %s/build/IPKG_BUILD/root/jgd215/", argv[1], argv[2], argv[1]);
    system(formats);

    //拷贝cgi程序
    snprintf (formats, sizeof(formats) - 1, "cp -rfd %s/compile_res/%s/app/bin/app_%s_CGI.out %s/build/IPKG_BUILD/root/jgd215/lighttpd/www/htdocs/cgi-bin/", argv[1], argv[2], argv[2], argv[1]);
    system(formats);

    //拷贝html程序
    snprintf (formats, sizeof(formats) - 1, "cp -rfd %s/interface/cgi/*.html %s/build/IPKG_BUILD/root/jgd215/lighttpd/www/htdocs/", argv[1], argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "cp -rfd %s/compile_res/%s/app/bin/app_%s.out %s/build/IPKG_BUILD/root/jgd215/", argv[1], argv[2], argv[2], argv[1]);
    system(formats);

    snprintf (formats, sizeof(formats) - 1, "cp -rfd %s/scripts/S99jgd215_install.sh %s/build/", argv[1], argv[1]);
    system(formats);

    /*填充data.tar.gz*/
    snprintf (formats, sizeof(formats) - 1, "cd %s/build/IPKG_BUILD/; tar -cvzf data.tar.gz ./*; cd -;", argv[1]);
    system(formats);

    /*获取git版本号*/
    systemArgs(version, "git log --oneline|awk 'NR==1{print}'", "", getVersion);

    fprintf (stderr, "git version:%s\n", version);

    /*填充control配置*/
    {
        snprintf (formats, sizeof(formats) - 1, 
                "cd %s/build/IPKG_BUILD/;   \
                echo \"Package: jgd215\" > control;     \
                echo \"Version: v0.5.16_%s\" >> control;        \
                echo \"Depends: \" >> control;  \
                echo \"Source: \" >> control;   \
                echo \"Section: \" >> control;  \
                echo \"Architecture: arm\" >> control; \
                echo \"Installed-Size: `du data.tar.gz|awk  '{print $1}'`\" >> control;\
                echo \"Description: none\" >> control; \
                pwd;\
                tar -czvf control.tar.gz ./control; \
                cd -;", argv[1], version);
        system(formats);
    }

    /*填充debian-binary*/
    {
        snprintf (formats, sizeof(formats) - 1, 
                "cd %s/build/IPKG_BUILD/;   \
                echo \"2.0\" > debian-binary; \
                cd -;", argv[1]);
        system(formats);
    }

    /*package ipk*/
    {
        snprintf (formats, sizeof(formats) - 1, 
                "cd %s/build/IPKG_BUILD/;   \
                tar -czvf jgd215.tar.gz ./data.tar.gz ./control.tar.gz ./debian-binary; \
                mv jgd215.tar.gz jgd215.ipk; \
                cd -;", argv[1]);
        system(formats);
    }

    return 0;
}
