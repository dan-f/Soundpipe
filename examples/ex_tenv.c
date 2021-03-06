#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "soundpipe.h"

typedef struct {
    sp_osc *osc;
    sp_ftbl *ft;
    sp_tevent *te;
    sp_metro *met;
    sp_tenv *tenv;
    SPFLOAT freq;
} udata;

void write_osc(sp_data *data, void *ud) {
    udata *udp = ud;
    SPFLOAT trig = 0;
    SPFLOAT env = 0;
    SPFLOAT osc = 0;
    sp_metro_compute(data, udp->met, NULL, &trig);
    sp_tevent_compute(data, udp->te, &trig, &udp->osc->freq);
    sp_osc_compute(data, udp->osc, NULL, &osc);
    sp_tenv_compute(data, udp->tenv, &trig, &env);

    data->out[0] = osc * env;
}

void freq_reinit(void *ud){
    SPFLOAT *freq = ud;
    *freq = 500 + rand() % 2000;
}

void freq_compute(void *ud, SPFLOAT *out){
    SPFLOAT *freq = ud;
    *out = *freq;
}

int main() {
    srand(time(NULL));
    udata ud;
    SPFLOAT *freqp = &ud.freq;
    ud.freq = 400;
    sp_data *sp;
    sp_create(&sp);

    sp_tenv_create(&ud.tenv);
    sp_metro_create(&ud.met);
    sp_tevent_create(&ud.te);
    sp_ftbl_create(sp, &ud.ft, 2048);
    sp_osc_create(&ud.osc);

    sp_tenv_init(sp, ud.tenv);
    ud.tenv->atk = 0.03;
    ud.tenv->hold = 0.01;
    ud.tenv->rel = 0.1;
    sp_metro_init(sp, ud.met);
    ud.met->freq = 3;
    sp_tevent_init(sp, ud.te, freq_reinit, freq_compute, freqp);
    sp_gen_sine(sp, ud.ft);
    sp_osc_init(sp, ud.osc, ud.ft, 0);
    ud.osc->freq = *freqp;
    sp->len = 44100 * 5;
    sp_process(sp, &ud, write_osc);

    sp_tenv_destroy(&ud.tenv);
    sp_metro_destroy(&ud.met);
    sp_ftbl_destroy(&ud.ft);
    sp_osc_destroy(&ud.osc);
    sp_tevent_destroy(&ud.te);
    sp_destroy(&sp);
    return 0;
}
