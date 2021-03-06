/*
 *  Copyright (c) 2019, Sensirion AG
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither the name of Sensirion AG nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "wiced.h"
#include "sps30.h"
#include "sensirion_common_wiced.h"

// this is a pretty direct port of sps30_example_usage.c

void application_start( )
{
    struct sps30_measurement m;
    u8 auto_clean_days = 4;
    u16 data_ready;
    s16 ret;

    wiced_init();

    sensirion_wiced_set_i2c_port(WICED_I2C_1); // adjust this to your hardware!

    WPRINT_APP_INFO(("SPS demo\n"));

    while (sps30_probe() != 0) {
        WPRINT_APP_INFO(("SPS sensor probing failed\n"));
        wiced_rtos_delay_milliseconds(1000);
    }
    WPRINT_APP_INFO(("SPS sensor probing successful\n"));

   ret = sps30_set_fan_auto_cleaning_interval_days(auto_clean_days);
   if (ret)
       printf("error %d setting the auto-clean interval\n", ret);

   ret = sps30_start_measurement();
   if (ret < 0)
       WPRINT_APP_INFO(("error starting measurement\n"));
   WPRINT_APP_INFO(("measurements started\n"));
   wiced_rtos_delay_milliseconds(1000);

   do {
       ret = sps30_read_data_ready(&data_ready);
       if (ret < 0) {
           WPRINT_APP_INFO(("error %d reading data-ready flag\n", ret));
       }

       if (data_ready) {
           ret = sps30_read_measurement(&m);
           if (ret < 0) {
               WPRINT_APP_INFO(("error reading measurement\n"));
           } else {
               WPRINT_APP_INFO(("measured values:\n"
                      "\t%0.2f pm1.0\n"
                      "\t%0.2f pm2.5\n"
                      "\t%0.2f pm4.0\n"
                      "\t%0.2f pm10.0\n"
                      "\t%0.2f nc0.5\n"
                      "\t%0.2f nc1.0\n"
                      "\t%0.2f nc2.5\n"
                      "\t%0.2f nc4.5\n"
                      "\t%0.2f nc10.0\n"
                      "\t%0.2f typical particle size\n\n",
                      m.mc_1p0, m.mc_2p5, m.mc_4p0, m.mc_10p0,
                      m.nc_0p5, m.nc_1p0, m.nc_2p5, m.nc_4p0, m.nc_10p0,
                      m.typical_particle_size));
           }
       }

       wiced_rtos_delay_milliseconds(1000);
   } while (1);

}
