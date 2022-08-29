/* Speech Recognition HMM Grammar Vocabulary Description file
 * Copyright (C) 2003-2022 Sensory, Inc. All Rights Reserved.
 * 
 *
 *            source: ./tmp/3XKH1jh4O4fnD2h5cNpVr0mIxg13/2126909838/trecs-en_US_12-11-0-1_cd4e139c27bc363a11a1e638e98c9f4314fc7bce.snsr
 *           created: Fri Aug 19 13:13:54 2022
 *   min lib version: 6.5.0
 *   operating point: 10
 *  production ready: NO - development only
 *       license key: yes
 * recognition limit: 107
 *    duration limit: 11.43 hours
 *
 * Created by VoiceHub 1.23.0
 * Project: Sensory_Demo_-_Voice_Genie_wakeword
 *
 * This model will stop working after a preset number of recognition events
 * and/or a after a preset number of audio "bricks" have been processed.
 *
 * ------------------------- DO NOT USE IN A PRODUCT -------------------------
 */

//extern u32 gs_wakeword_grammarLabel;
#ifndef NETLABEL
#define NETLABEL
//extern u32 dnn_wakeword_netLabel;
#endif

/* The following phrases (Hex format) correspond to the word IDs emitted by the recognizer. */
#define WAKEWORD_PHRASE_COUNT 3
#define WAKEWORD_PHRASE_0 "SILENCE"	/* Legacy system phrase */
#define WAKEWORD_PHRASE_1 "Voice Genie"	/* Phrase: voice_genie */
#define WAKEWORD_PHRASE_2 "nota"	/* Legacy system phrase */


/* The following identifier was automatically generated by add_model.py */
#define MODEL_NAME "voice_genie"

/* Speech Recognition HMM Grammar file V60 for pc60 platform
 * Copyright (C) 2003-2022 Sensory, Inc. All Rights Reserved.
 * 
 *
 *            source: ./tmp/3XKH1jh4O4fnD2h5cNpVr0mIxg13/2126909838/trecs-en_US_12-11-0-1_cd4e139c27bc363a11a1e638e98c9f4314fc7bce.snsr
 *           created: Fri Aug 19 13:13:54 2022
 *   min lib version: 6.5.0
 *   operating point: 10
 *  production ready: NO - development only
 *       license key: yes
 * recognition limit: 107
 *    duration limit: 11.43 hours
 *
 * Created by VoiceHub 1.23.0
 * Project: Sensory_Demo_-_Voice_Genie_wakeword
 *
 * This model will stop working after a preset number of recognition events
 * and/or a after a preset number of audio "bricks" have been processed.
 *
 * ------------------------- DO NOT USE IN A PRODUCT -------------------------
 */

const char *wakeword_phrases[] = {WAKEWORD_PHRASE_0, WAKEWORD_PHRASE_1, WAKEWORD_PHRASE_2};
const unsigned short gs_wakeword_grammarLabel[] = {

   60,	 // 0x003c
  217,	 // 0x00d9
   24,	 // 0x0018
    0,	 // 0x0000
    1,	 // 0x0001
   25,	 // 0x0019
   32,	 // 0x0020
    0,	 // 0x0000
   32,	 // 0x0020
  179,	 // 0x00b3
  231,	 // 0x00e7
 1000,	 // 0x03e8
    0,	 // 0x0000
   27,	 // 0x001b
  200,	 // 0x00c8
 1000,	 // 0x03e8
  300,	 // 0x012c
    1,	 // 0x0001
   36,	 // 0x0024
   12,	 // 0x000c
    2,	 // 0x0002
    2,	 // 0x0002
   24,	 // 0x0018
   24,	 // 0x0018
    7,	 // 0x0007
    1,	 // 0x0001
    0,	 // 0x0000
    2,	 // 0x0002
    0,	 // 0x0000
    0,	 // 0x0000
    1,	 // 0x0001
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
28771,	 // 0x7063
  102,	 // 0x0066
  104,	 // 0x0068
  176,	 // 0x00b0
  200,	 // 0x00c8
  204,	 // 0x00cc
  208,	 // 0x00d0
  256,	 // 0x0100
  304,	 // 0x0130
  332,	 // 0x014c
    0,	 // 0x0000
  340,	 // 0x0154
  344,	 // 0x0158
  346,	 // 0x015a
    0,	 // 0x0000
  442,	 // 0x01ba
 1700,	 // 0x06a4
    2,	 // 0x0002
32769,	 // 0x8001
    4,	 // 0x0004
32771,	 // 0x8003
    1,	 // 0x0001
32768,	 // 0x8000
    7,	 // 0x0007
32773,	 // 0x8005
    8,	 // 0x0008
32774,	 // 0x8006
    5,	 // 0x0005
32771,	 // 0x8003
    6,	 // 0x0006
32772,	 // 0x8004
   11,	 // 0x000b
32778,	 // 0x800a
    8,	 // 0x0008
32775,	 // 0x8007
   10,	 // 0x000a
32777,	 // 0x8009
   14,	 // 0x000e
32781,	 // 0x800d
   13,	 // 0x000d
32780,	 // 0x800c
   17,	 // 0x0011
32784,	 // 0x8010
   16,	 // 0x0010
32783,	 // 0x800f
   20,	 // 0x0014
32787,	 // 0x8013
   19,	 // 0x0013
32786,	 // 0x8012
   23,	 // 0x0017
32790,	 // 0x8016
   22,	 // 0x0016
32789,	 // 0x8015
  118,	 // 0x0076
  111,	 // 0x006f
  105,	 // 0x0069
   99,	 // 0x0063
  101,	 // 0x0065
   95,	 // 0x005f
  103,	 // 0x0067
  101,	 // 0x0065
  110,	 // 0x006e
  105,	 // 0x0069
  101,	 // 0x0065
    0,	 // 0x0000
   23,	 // 0x0017
65535,	 // 0xffff
    0,	 // 0x0000
    0,	 // 0x0000
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
22144,	 // 0x5680
 2678,	 // 0x0a76
 1926,	 // 0x0786
 2041,	 // 0x07f9
 1793,	 // 0x0701
 1975,	 // 0x07b7
 1289,	 // 0x0509
 1420,	 // 0x058c
 1612,	 // 0x064c
 1836,	 // 0x072c
 1623,	 // 0x0657
 1523,	 // 0x05f3
 1726,	 // 0x06be
 1795,	 // 0x0703
 1749,	 // 0x06d5
 1964,	 // 0x07ac
 2009,	 // 0x07d9
 2105,	 // 0x0839
 2411,	 // 0x096b
 2661,	 // 0x0a65
 2887,	 // 0x0b47
 3142,	 // 0x0c46
 3136,	 // 0x0c40
 2986,	 // 0x0baa
 2852,	 // 0x0b24
    1,	 // 0x0001
    5,	 // 0x0005
    1,	 // 0x0001
    4,	 // 0x0004
    1,	 // 0x0001
    2,	 // 0x0002
    1,	 // 0x0001
    3,	 // 0x0003
    1,	 // 0x0001
    7,	 // 0x0007
    2,	 // 0x0002
    7,	 // 0x0007
    1,	 // 0x0001
    8,	 // 0x0008
  173,	 // 0x00ad
    1,	 // 0x0001
    5,	 // 0x0005
    1,	 // 0x0001
    0,	 // 0x0000
65535,	 // 0xffff
 1403,	 // 0x057b
   34,	 // 0x0022
  232,	 // 0x00e8
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
   34,	 // 0x0022
  184,	 // 0x00b8
   40,	 // 0x0028
  112,	 // 0x0070
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
   34,	 // 0x0022
  232,	 // 0x00e8
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
   46,	 // 0x002e
  212,	 // 0x00d4
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
   50,	 // 0x0032
  310,	 // 0x0136
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
   72,	 // 0x0048
  335,	 // 0x014f
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
   33,	 // 0x0021
  128,	 // 0x0080
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
   25,	 // 0x0019
  384,	 // 0x0180
    9,	 // 0x0009
32768,	 // 0x8000
65535,	 // 0xffff
    0,	 // 0x0000
    0,	 // 0x0000
    8,	 // 0x0008
    2,	 // 0x0002
    0,	 // 0x0000
    0,	 // 0x0000
    0,	 // 0x0000
    7,	 // 0x0007
32770,	 // 0x8002
32772,	 // 0x8004
   16,	 // 0x0010
  513,	 // 0x0201
   12,	 // 0x000c
32774,	 // 0x8006
    2,	 // 0x0002
   16,	 // 0x0010
  642,	 // 0x0282
   15,	 // 0x000f
32776,	 // 0x8008
    2,	 // 0x0002
    0,	 // 0x0000
    1,	 // 0x0001
   11,	 // 0x000b
    7,	 // 0x0007
    3,	 // 0x0003
    0,	 // 0x0000
    3,	 // 0x0003
   14,	 // 0x000e
    8,	 // 0x0008
    4,	 // 0x0004
    0,	 // 0x0000
    1,	 // 0x0001
   10,	 // 0x000a
    9,	 // 0x0009
32778,	 // 0x800a
    0,	 // 0x0000
    2,	 // 0x0002
   13,	 // 0x000d
    9,	 // 0x0009
32780,	 // 0x800c
   16,	 // 0x0010
 1024,	 // 0x0400
   22,	 // 0x0016
32782,	 // 0x800e
32784,	 // 0x8010
    0,	 // 0x0000
    0,	 // 0x0000
   24,	 // 0x0018
   11,	 // 0x000b
    9,	 // 0x0009
    0,	 // 0x0000
    0,	 // 0x0000
   23,	 // 0x0017
   12,	 // 0x000c
32786,	 // 0x8012
   16,	 // 0x0010
  256,	 // 0x0100
    6,	 // 0x0006
32788,	 // 0x8014
   11,	 // 0x000b
    0,	 // 0x0000
    1,	 // 0x0001
    5,	 // 0x0005
   14,	 // 0x000e
   12,	 // 0x000c
    0,	 // 0x0000
    0,	 // 0x0000
    4,	 // 0x0004
   15,	 // 0x000f
32790,	 // 0x8016
   16,	 // 0x0010
  772,	 // 0x0304
   18,	 // 0x0012
32792,	 // 0x8018
   14,	 // 0x000e
    0,	 // 0x0000
    4,	 // 0x0004
   17,	 // 0x0011
   17,	 // 0x0011
   15,	 // 0x000f
    0,	 // 0x0000
    4,	 // 0x0004
   16,	 // 0x0010
   18,	 // 0x0012
32794,	 // 0x801a
   16,	 // 0x0010
  133,	 // 0x0085
    3,	 // 0x0003
32796,	 // 0x801c
   17,	 // 0x0011
    0,	 // 0x0000
    6,	 // 0x0006
    2,	 // 0x0002
   20,	 // 0x0014
   18,	 // 0x0012
    0,	 // 0x0000
    5,	 // 0x0005
    1,	 // 0x0001
   21,	 // 0x0015
32798,	 // 0x801e
   16,	 // 0x0010
  899,	 // 0x0383
   21,	 // 0x0015
32800,	 // 0x8020
   20,	 // 0x0014
    0,	 // 0x0000
    3,	 // 0x0003
   20,	 // 0x0014
   23,	 // 0x0017
   21,	 // 0x0015
    0,	 // 0x0000
    3,	 // 0x0003
   19,	 // 0x0013
65535,	 // 0xffff
32802,	 // 0x8022
};
