#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <dirent.h>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define PURPLE "\033[38;2;128;0;255m"
#define RESET "\033[0m"

#define NMEA_SOCKET "/var/run/gps-share.sock"

char ddmm[32];
char dddmm[32];
int g_argc;
char **g_argv;
int script_mode = 0;
char file_path[512];
int port = 5000;
char speed[32] = "0.0";
char version[32] = "v4.0";
int verbose = 0;
int use_avahi = 0;
char socket_path[512] = NMEA_SOCKET;
char parrent_path[512];
char ascii_art_path[1024];
char exe_real[1024];
int log_index = 0;
char log_list[256][256];

void super_vomit_logs(){
    for(int i = 0; i < log_index; i++){
        printf("%s", log_list[i]);
    }
    log_index = 0;
}


int locate(){
    ssize_t n = readlink("/proc/self/exe", exe_real, sizeof(exe_real) - 1);
    if (n <= 0){return 0;}
    exe_real[n] = '\0';

    char *slash = strrchr(exe_real, '/');
    if(slash){
        *slash = '\0';
    }else{
        return 0;
    }
    slash = strrchr(exe_real, '/');
    if(slash){
        *slash = '\0';
    }else{
        return 0;
    }
    snprintf(parrent_path, sizeof(parrent_path), "%s", exe_real);
    snprintf(ascii_art_path, sizeof(ascii_art_path), "%s/assets/ascii", parrent_path);
    return 1;
}



int random_art(const char *size, char *color){
    int n = strlen(size);
    char *size_result[8];
    for(int x = 0; x < n; x++){
        if(size[x] == 's'){size_result[x] = "small";}
        else if(size[x] == 'm'){size_result[x] = "medium";}
        else if(size[x] == 'b'){size_result[x] = "big";}
    }

    int count_art = 0;
    char size_path[1024];
    char path[1024];
    snprintf(size_path, sizeof(size_path), "%s/%s", ascii_art_path, size_result[rand()%n]);
    DIR *d = opendir(size_path);
    if(!d){printf("%s[!]%s Cannot load ascii arts\n", YELLOW, RESET); closedir(d); return 0;}
    struct dirent *e;
    int idx = 0;
    while((e = readdir(d)) != NULL){
        if(e->d_name[0] == '.'){continue;}
        count_art ++;
    }
    rewinddir(d);
    if(count_art == 0){printf("%s[!]%s Cannot load ascii arts\n", YELLOW, RESET); closedir(d); return 0;}
    int pick = rand() % count_art;
    while((e = readdir(d)) != NULL){
        if(e->d_name[0] == '.'){continue;}
        if(idx == pick){
            snprintf(path, sizeof(path), "%s/%s", size_path, e->d_name);
            break;
            }
        idx++;
    }

    FILE *f = fopen(path, "r");
    if(!f){printf("%s[!]%s Cannot read random ascii art file\n", YELLOW, RESET); closedir(d); return 0;}
    int c;
    printf("%s", color);
    while((c = fgetc(f)) != EOF){putchar(c);}
    printf("%s", RESET);
    printf("\n");
    fclose(f);

    return 1;

}


void anti_injection(char *raw, int lv){
    if(lv == 0){
        const char *lv0 = 
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789/._-";

        if(strspn(raw, lv0) != strlen(raw)){
            random_art("smb", RED);
            printf("%s[x] There is nothing injectable, you hacker%s\n", PURPLE, RESET);
            exit(1);
        }
    }else if(lv == 1){
        const char *lv1 = 
        "<>:;\"'[]{}?/\\+=()*&^%$#@!~|\n` ";

        if(strpbrk(raw, lv1) != NULL){
            random_art("smb", RED);
            printf("%s[x] There is nothing injectable, you hacker%s\n", PURPLE, RESET);
            exit(1);
        }
    }else if(lv == 2){
        const char *lv2 = 
        "<>;$#*?'\"\\|&@!";

        if(strpbrk(raw, lv2) != NULL){
            random_art("smb", RED);
            printf("%s[x] There is nothing injectable, you hacker%s\n", PURPLE, RESET);
            exit(1);
        }
    }
}

enum{
    OPT_AVAHI = 1000,
    OPT_SOCKET_PATH,
};

static struct option long_options[] = {
    {"avahi", no_argument, 0, OPT_AVAHI},
    {"script", required_argument, 0, 's'},
    {"verbose", no_argument, 0, 'v'},
    {"version", no_argument, 0, 'V'},
    {"help", no_argument, 0, 'h'},
    {"port", required_argument, 0, 'p'},
    {"clean", no_argument, 0, 'c'},
    {"socket-path", required_argument, 0, OPT_SOCKET_PATH}
};


const char *tipslib[] = {
"Help, there's someone in the ocean. It's you.",
"You are faking GPS. The satellites are watching.",
"Coordinates spoofed. Karma stays put.",
"You can fake your location, but not your alibi.",
"Somewhere, a cartographer just felt a disturbance.",
"You moved 8,000 km in 3 seconds. INTERPOL is here.",
"Current location: Null Island. Population: you.",
"GPS says you're in Antarctica. Your boss says you're late.",
"Satellites don't judge. They just log. Forever.",
"You are not lost. You are geographically obfuscated.",
"Fake GPS enabled. Sense of direction still unpatched.",
"Teleport into Area 51. Support cannot help you.",
"Your location is fictional. Resemblance is coincidental.",
"The map is not the territory. Parole disagrees.",
"You can hide from the map, but not from the timeline.",
"You are here. Allegedly.",
"There's no place like 127.0.0.1. GPS disagrees.",
"Warning: fake location restricted. Pigeons are armed.",
"You reached your destination. It's a lie.",
"GPS accuracy: +/-5 meters. Your excuses: +/-5 kilometers.",
"Sharks don't respect EULAs.",
"You are currently at 0N 0E. Bring a towel.",
"Fake GPS: admitting you're lost is a vulnerability.",
"The satellites see you. They are not your friends.",
"You are everywhere. Your calendar is not amused.",
"Location spoofed. Responsibility not spoofed.",
"Somewhere in Nevada, a clipboard just got another name.",
"You can't spell navigation without nope.",
"The ocean is a great place to fake being. It's wet.",
"Fake GPS won't save you from real deadlines.",
"Satellites are like git: reflog remembers.",
"The Bermuda Triangle called. It wants its tourists back.",
"You are in international waters. Responsibilities are not.",
"Location set to somewhere else. Feelings unchanged.",
"The satellites have a group chat. You're the meme.",
"You can spoof GPS, but not the questions.",
"Fake GPS: now you can be lost in 4K.",
"You are 0 meters from your fake destination.",
"Warning: spoofing near a base may summon men in black.",
"Null Island: where all bad coordinates go to be judged.",
"GPS says you're here. Your soul says otherwise.",
"Pigeons have better location services than you do.",
"Karma stays put. You don't.",
"This NMEA sentence is 100% fiction. Geoclue believes it.",
"Three satellites. One lie. Infinite confidence.",
"The map is a machine you are lying to.",
"This coordinate was picked by an array index.",
"You are where you told your laptop to be.",
"NMEA: Nobody's Meaningful Evidence of location.",
"Your phone believes 500m. Your legs disagree.",
"Accuracy says 3m. Your claim's accuracy is zero.",
"Every fix you have ever received was a rumour until now.",
"Doppler says 0.0 knots. You are a very stationary liar.",
"You spoofed a satellite. It wasn't informed.",
"Somewhere a real person is being mislocated because of you.",
"Coordinates are a story. You are writing the ending.",
"Waypoint acquired. Not verified. Never verified.",
"This latitude has no relationship to your life.",
"Your position is now a work of fiction with a checksum.",
"You did not navigate. You narrated.",
"A fake GPS, read seriously. We are both at fault.",
"You are now in another country. Tell no one.",
"The map will update in 3 seconds. You will not. Move on.",
"Two hours walking, saved by four numbers.",
"Friends will ask why you moved. No answer.",
"Speed 0.0 knots, yet you always arrive.",
"Stationary an hour, per a device you lied to.",
"The pigeon on your sill knows. Not fooled.",
"GPS stands for Guilty People's Story.",
"You are the storm that never was.",
"Somewhere, a pigeon is laughing at you. This is documented.",
"Your history now includes a country. Unvisited.",
"You achieved what climbers train for: lying flat.",
"The coordinates are correct. Nothing else is.",
"Welcome to the simulation. One index wide.",
};



char *lat_to_ddmm(double raw){
    int dd = (int)raw;
    double mm = (raw - dd) * 60;
    snprintf(ddmm, sizeof(ddmm), "%02d%07.4f", dd, mm);
    return ddmm;
}

char *lon_to_dddmm(double raw){
    int ddd = (int)raw;
    double mm = (raw - ddd) * 60;
    snprintf(dddmm, sizeof(dddmm), "%03d%07.4f", ddd, mm);
    return dddmm;
}


void init(){

    super_vomit_logs();

    system("pkill -9 -f [a]vahi-publish-service >/dev/null; pkill -9 -f [s]ocat >/dev/null");
    printf("%s[*]%s All redundant server cleared\n", BLUE, RESET);

    if(locate() == 0){
        printf("%s[!]%s Cannot locate project root\n", YELLOW, RESET);
    }else{
        printf("%s[*]%s Located project root\n", BLUE, RESET);
    }

    int avahi_result = 1;
    int socat_avahi_result = 1;
    int socat_unix_result = 1;
    log_index = 0;

    char child_cmd[1024];
    child_cmd[0] = '\0';
    if(script_mode == 1){
        snprintf(child_cmd, sizeof(child_cmd), "%s -s %s", g_argv[0], file_path);
    }else if(script_mode == 0){
        strcat(child_cmd, g_argv[0]);
        strcat(child_cmd, " ");
        for(int i = optind; i < g_argc; i++){
            strcat(child_cmd, g_argv[i]);
            strcat(child_cmd, " ");
        }
        child_cmd[strlen(child_cmd) - 1] = '\0';
    }
    

    if(use_avahi == 1){

    char hang_avahi[512];
    snprintf(hang_avahi, sizeof(hang_avahi), "avahi-publish-service -H archlinux.local \"fakegps\" _nmea-0183._tcp %d >/dev/null 2>&1 &", port);
    system(hang_avahi);
    sleep(1);
    avahi_result = system("avahi-browse -rt _nmea-0183._tcp 2>/dev/null | grep -q fakegps");
    if(avahi_result == 0){
        printf("%s[+]%s Avahi-publish-service up\n", GREEN, RESET);
    }else{
        printf("%s[-]%s Failed to hang up avahi-publish-service\n", RED, RESET);
    }

    char hang_socat_avahi[2048];
    snprintf(hang_socat_avahi, sizeof(hang_socat_avahi), "socat TCP-LISTEN:%d,reuseaddr,fork EXEC:'env FAKEGPS_CHILD=1 %s' >/dev/null 2>&1 &", port, child_cmd);
    system(hang_socat_avahi);
    char socat_avahi_check[256];
    snprintf(socat_avahi_check, sizeof(socat_avahi_check), "ss -ltn | grep -q 'LISTEN.*:%d'", port);
    sleep(1);
    socat_avahi_result = system(socat_avahi_check);
    if(socat_avahi_result == 0){
        printf("%s[+]%s Socat connected at TCP %d\n", GREEN, RESET, port);
    }else{
        printf("%s[-]%s Failed to connect to socat at TCP %d\n", RED, RESET, port);
    }

}else if(use_avahi == 0){
    char hang_socat_unix[4096];

    snprintf(hang_socat_unix, sizeof(hang_socat_unix), "rm -f %s; socat UNIX-LISTEN:%s,perm=0666,fork EXEC:'env FAKEGPS_CHILD=1 %s' >/dev/null 2>&1 &", socket_path, socket_path, child_cmd);
    system(hang_socat_unix);

    char socat_unix_check[1024];
    snprintf(socat_unix_check, sizeof(socat_unix_check), "test -S %s", socket_path);
    sleep(1);
    socat_unix_result = system(socat_unix_check); 

    if(socat_unix_result == 0){
        printf("%s[+]%s Created socket %s\n", GREEN, RESET, socket_path);
    }else{
        char check_socket_permission[2048];
        snprintf(check_socket_permission, sizeof(check_socket_permission), "touch %s.check >/dev/null 2>&1 && rm -f %s.check >/dev/null 2>&1", socket_path, socket_path);
        if(system(check_socket_permission) == 0){
            printf("%s[-]%s Failed to create socket %s by unknown error\n", RED, RESET, socket_path);
        }else{
            printf("%s[-]%s Failed to create socket %s by permission denied. Try 'sudo fakegps' or use another path to socket(default is %s)\n", RED, RESET, socket_path, NMEA_SOCKET);
        }
    }
}
    if((avahi_result == 0 && socat_avahi_result == 0) || socat_unix_result == 0){
        printf("%s[+]%s Initial completed\n", GREEN, RESET);
    }else{
        printf("%s[-]%s Initial failed\n", RED, RESET);
        system("pkill -9 -f [a]vahi-publish-service >/dev/null; pkill -9 -f [s]ocat >/dev/null");
        exit(1);
    }
}

    

void output(char ns, char *lat_dm, char ew, char *lon_dm, char *speed){
 
    long now = time(NULL);
    struct tm *u = gmtime(&now);

    char current_time[32];
    snprintf(current_time, sizeof(current_time),"%02d%02d%02d.000", u->tm_hour, u->tm_min, u->tm_sec); 

    char date[32];
    snprintf(date,sizeof(date),"%02d%02d%02d",u->tm_mday, u->tm_mon + 1, u->tm_year % 100);

    char body[128];
    snprintf(body, sizeof(body), "GPRMC,%s,A,%s,%c,%s,%c,%s,0.0,%s,,,A", current_time, lat_dm, ns, lon_dm, ew, speed, date);

    unsigned char checksum = 0;
    for(int i = 0; body[i] != '\0'; i++){
        checksum ^= (unsigned char)body[i];
    }
    
    char gprmc[256];
    snprintf(gprmc, sizeof(gprmc), "$%s*%02X\r\n", body, checksum);

    printf("%s",gprmc);
    fflush(stdout);
    sleep(1);

}

void boom(){
    const char *boom[] = {
        "                                                                      \n",
        " ▄▄▄▄▄▄▄▄            ▄▄                     ▄▄▄▄   ▄▄▄▄▄▄      ▄▄▄▄   \n",
        " ██▀▀▀▀▀▀            ██                   ██▀▀▀▀█  ██▀▀▀▀█▄  ▄█▀▀▀▀█  \n",
        " ██         ▄█████▄  ██ ▄██▀    ▄████▄   ██        ██    ██  ██▄      \n",
        " ███████    ▀ ▄▄▄██  ██▄██     ██▄▄▄▄██  ██  ▄▄▄▄  ██████▀    ▀████▄  \n",
        " ██        ▄██▀▀▀██  ██▀██▄    ██▀▀▀▀▀▀  ██  ▀▀██  ██             ▀██ \n",
        " ██        ██▄▄▄███  ██  ▀█▄   ▀██▄▄▄▄█   ██▄▄▄██  ██        █▄▄▄▄▄█▀ \n",
        " ▀▀         ▀▀▀▀ ▀▀  ▀▀   ▀▀▀    ▀▀▀▀▀      ▀▀▀▀   ▀▀         ▀▀▀▀▀   \n",
        "                                                                      \n",
        "                                                                      \n",
        NULL                                               
    };
    for(int i = 0; boom[i] != NULL; i++){
        printf("%s", boom[i]);
    }

    random_art("b", BLUE);

    int tip_n= (sizeof(tipslib)/sizeof(tipslib[0]));
    int t1 = rand() % tip_n;
    int t2 = rand() % tip_n;
    if(t1 == t2){t2 = (t2 + 1) % tip_n;}

    printf("\n");
    printf("         =[ %sfakegps %s%s   --by yuzuki                                     ]\n", YELLOW, version, RESET);
    printf("  + -- --=[ %-62s ]\n", tipslib[t1]);
    printf("  + -- --=[ %-62s ]\n", tipslib[t2]);
    printf("\n");
}

void print_usage(){
    super_vomit_logs();
    printf("Usage: fakegps [option(s)] <N/S> <lat> <E/W> <lon>\n");
    exit(1);
}

void print_usage_detail(){
    printf("Usage: fakegps [option(s)] <N/S> <lat> <E/W> <lon>\n");
    printf("Options:\n    %-33sChoose a routefile to change gps position in script mode\n    %-33sChoose a port which avahi&socat used in\n    %-33sForce clear all the fakegps server\n    %-33sShow this detail help\n    %-33sEcho current version\n    %-33sVerbose the fake NMEA flow which sent to Geoclue\n    %-33sUse avahi-publish-service to spread fake NMEA flow to the whole WLAN\n    %-33sUse a custom unix socket path(default is /var/run/gps-share.sock), notice only the geoclue.conf be changed as well can make sense\n", "-s, --script ROUTEFILE", "-p, --port PORT", "-c, --clean", "-h, --help", "-V, --version", "-v, --verbose", "    --avahi", "    --socket-path");
    exit(0);
}


void resolve_options(){
    int opt;
    while((opt = getopt_long(g_argc,g_argv,":s:p:chVv", long_options, NULL)) != -1){
        if(optarg != NULL){anti_injection(optarg, 0);}
        switch(opt){
            case 's': script_mode = 1; snprintf(file_path,sizeof(file_path),"%s", optarg); break;
            case 'p': port = atoi(optarg);break;
            case 'c': system("pkill -9 -f [a]vahi-publish-service >/dev/null; pkill -9 -f [s]ocat >/dev/null"); printf("%s[*]%s All redundant server cleared\n", BLUE, RESET); exit(0);break;
            case 'h': print_usage_detail();break;
            case 'V': printf("fakegps %s\n", version);exit(0); break;
            case 'v': verbose = 1;break;
            case OPT_AVAHI : use_avahi = 1; break;
            case OPT_SOCKET_PATH : snprintf(socket_path, sizeof(socket_path), "%s", optarg); break;
            case '?': if(log_index >= (int)(sizeof(log_list)/sizeof(log_list[0]))){
                            printf("%s[-]%s Too many bad parameter or options (%d). Unbelievable\n", RED, RESET, log_index); super_vomit_logs(); exit(1); break;
                        }else{
                            snprintf(log_list[log_index++], sizeof(log_list[0]), "%s[!]%s Unknown parameter '%s'\n", YELLOW, RESET, g_argv[optind-1]?g_argv[optind-1]:"(?)");break;
                        }
            case ':': if(log_index >= (int)(sizeof(log_list)/sizeof(log_list[0]))){
                            printf("%s[-]%s Too many bad parameter or options (%d). Unbelievable\n", RED, RESET, log_index); super_vomit_logs(); exit(1); break;
                        }else{
                            snprintf(log_list[log_index++], sizeof(log_list[0]), "%s[-]%s Option '%s' requires an argument\n", RED, RESET, g_argv[optind-1]?g_argv[optind-1]:"(?)");exit(1);break;
                        }
            default : if(log_index >= (int)(sizeof(log_list)/sizeof(log_list[0]))){
                            printf("%s[-]%s Too many bad parameter or options (%d). Unbelievable\n", RED, RESET, log_index); super_vomit_logs(); exit(1); break;
                        }else{
                            snprintf(log_list[log_index++], sizeof(log_list[0]), "%s[-]%s Unknown options error\n", RED, RESET);exit(1);break;
                        }
        }
    }
}




int main(int argc,char **argv){

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srand((unsigned)(time(NULL) ^ ts.tv_nsec ^ (getpid() << 10)));

    g_argc = argc;
    g_argv = argv;

    locate();
    resolve_options();

    int rest = argc - optind;

    if(script_mode == 0){
        if(
            (rest != 4) ||
            (argv[optind][0] != 'N' && argv[optind][0] != 'S') || 
            (atof(argv[optind+1]) < 0 || (atof(argv[optind+1]) > 90)) ||
            (argv[optind+2][0] != 'E' && argv[optind+2][0] != 'W') ||
            (atof(argv[optind+3]) < 0 || (atof(argv[optind+3]) > 180))
        ){
            print_usage();
        }
    }else{
        if(rest != 0){
            print_usage();
        }
    }

    if(script_mode == 1){
    FILE *check = fopen(file_path, "r");
        if (check == NULL){
            printf("%s[-]%s Routefile \"%s\" not found\n", RED, RESET, file_path);
            exit(1);
        }
        fclose(check);
    }

    if(getenv("FAKEGPS_CHILD") == NULL){

        boom();
        init();
        if(script_mode == 1){ printf("%s[+]%s Loaded routefile \"%s\"\n", GREEN, RESET, file_path);}
        if(verbose == 0){
            exit(0);
        }else if(verbose ==1){
            char cmd[1024];
            if(use_avahi == 1){
                snprintf(cmd, sizeof(cmd), "nc 127.0.0.1 %d", port);
            }else if(use_avahi == 0){
                snprintf(cmd, sizeof(cmd), "nc -U %s", socket_path);
            }
            
            FILE *fp = popen(cmd, "r");
                char line[512];
                while(fgets(line, sizeof(line), fp)){
                    line[strcspn(line, "\r\n")] = '\0';
                    printf("%s[+]%s Received: %s\n", GREEN, RESET, line);
                }
                pclose(fp);

        }

    }else if(strcmp(getenv("FAKEGPS_CHILD"),"1") == 0){

        if (script_mode == 0){
            double lat = atof(g_argv[optind+1]);
            double lon = atof(g_argv[optind+3]);
            char ns = g_argv[optind][0];
            char ew = g_argv[optind+2][0];

            char *lat_dm =lat_to_ddmm(lat);
            char *lon_dm =lon_to_dddmm(lon);

            while(1){
                output(ns,lat_dm,ew,lon_dm,speed);
            }
            
        }else if(script_mode == 1){
            while(1){
                FILE *f = fopen(file_path, "r");
                    if (f == NULL){
                        printf("%s[-]%s Routefile \"%s\" not found\n", RED, RESET, file_path);
                        exit(1);
                    }
                    
                    char line[256];
                    while(fgets(line, sizeof(line), f)){
                        line[strcspn(line, "\n")] = '\0';
                        char *ns = strtok(line,  ",");
                        char *lat = strtok(NULL, ",");
                        char *ew = strtok(NULL, ",");
                        char *lon = strtok(NULL, ",");
                        char *speed = strtok(NULL, ",");

                        if(lat == NULL) lat = "0.0";
                        if(lon == NULL) lon = "0.0";
                        if(speed == NULL) speed = "0.0";
                        if(ns == NULL || ew == NULL) continue;

                        char *lat_dm =lat_to_ddmm(atof(lat));
                        char *lon_dm =lon_to_dddmm(atof(lon));

                        output(*ns,lat_dm,*ew,lon_dm,speed);
                    }
                fclose(f);
            } 
        }
    }
}
