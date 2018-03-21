sound source localization on xmos
=====================

验证进度
-------

1. TDOA表格验证
2. FFT及IFFT验证

Summary
-------

|I2S| interfaces are key to many audio systems. XMOS technology is perfectly suited
to these applications - supporting a wide variety of standard interfaces and
also a large range of DSP functions.

This application note demonstrates the use of the XMOS |I2S| library to
create a digital audio loopback on an XMOS multicore microcontroller.

The code used in the application note configures the audio codecs to simultaneously
send and receive audio samples. It then uses the |I2S| library to
loopback all 8 channels.

Required tools and libraries
............................

 * xTIMEcomposer Tools - Version 14.0.0
 * XMOS |I2S|/TDM library - Version 2.1.0
 * XMOS GPIO library - Version 1.0.0
 * XMOS |I2C| library - Version 3.1.0

Required hardware
.................
The example code provided with the application has been implemented
and tested on the xCORE-200 Multichannel Audio Platform.

Prerequisites
..............
 * This document assumes familarity with |I2S| interfaces, the XMOS xCORE
   architecture, the XMOS tool chain and the xC language. Documentation related
   to these aspects which are not specific to this application note are linked
   to in the references appendix.

 * For a description of XMOS related terms found in this document
   please see the XMOS Glossary [#]_.

.. [#] http://www.xmos.com/published/glossary
