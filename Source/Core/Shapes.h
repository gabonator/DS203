#ifndef __SHAPES_H__
#define __SHAPES_H__

//TODO: Optimize

static const char corner_right[] = 
  "\x10"
  "................"
  "................"
  " ..............."
  "  .............."
  "     ..........."
  "           ....."
  "             ..."
  "              .."
  "               .";

static const char corner_left[] = 
  "\x10"
  "               ."
  "              .."
  "             ..."
  "           ....."
  "     ..........."
  "  .............."
  " ..............."
  "................"
  "................";

static const char more_left[] = 
  "\x0e"
  "      .       "
  "     ...      "
  "    .....     "
  "   .......    "
  "  .........   "
  " ...........  "
  "              "
  "              ";


static const char more_right[] = 
  "\x0e"
  "              "
  " ...........  "
  "  .........   "
  "   .......    "
  "    .....     "
  "     ...      "
  "      .       "
  "              ";

static const char updown[] = 
  "\x0e"
  "   .      .   "
  "  ..      ..  "
  " ............ "
  "  ..      ..  "
  "   .      .   ";

static const char leftright[] = 
  "\x0e"
  "      .       "
  "     ...      "
  "    .....     "
  "      .       "
  "      .       "
  "      .       "
  "    .....     "
  "     ...      "
  "      .       ";


static const char cha_base[] = 
  "\x09"
  "........."
  "........."
  ".. .... ."
  ".       ."
  "....... ."
  "........."
  "........."
  " ....... "
  "  .....  "
  "   ...   "
  "    .    ";

static const char trig_base[] = 
  "\x09"
  "........."
  ". ......."
  ". ......."
  ".       ."
  ". ......."
  ". ......."
  "........."
  " ....... "
  "  .....  "
  "   ...   "
  "    .    ";

static const char trig_time[] = 
  "\x05"
  ".    "
  "..   "
  "...  "
  ".... "
  "....."
  ".... "
  "...  "
  "..   "
  ".    ";


static const char chb_base[] = 
  "\x09"
  "........."
  ".. ...  ."
  ". ... . ."
  ". .. .. ."
  ". .. .. ."
  "..  ... ."
  "........."
  " ....... "
  "  .....  "
  "   ...   "
  "    .    ";

static const char sel_left[] = 
  "\x0e"
  "              "
  "              "
  "              "
  "              "
  "  ..........  "
  " ............ "
  ".............."
  "..............";

static const char sel_right[] = 
  "\x0e"
  ".............."
  ".............."
  " ............ "
  "  ..........  "
  "              "
  "              "
  "              "
  "              ";

static const char sig_dc[] = 
  "\x0e"
  "              "
  "     .  .     "
  "     .  .     "
  "     .  .     "
  "     .  .     "
  "        .     "
  "     .  .     "
  "     .  .     "
  "     .  .     "
  "     .  .     ";

static const char sig_ac[] = 
  "\x0e"
  "              "
  "     .    .   "
  "    .     .   "
  "   .      .   "
  "   .      .   "
  "    .     .   "
  "     .    .   "
  "      .   .   "
  "       .  .   "
  "       .  .   "
  "      .   .   ";

static const char sig_gnd[] = 
  "\x0e"
  "              "
  "      .       "
  "      . .     "
  "      . . .   "
  "      . . .   "
  " ...... . .   "
  "      . . .   "
  "      . . .   "
  "      . .     "
  "      .       "
  "              ";

static const char sig_dig[] = 
  "\x0e"
  "              "
  "         .    "
  "         .    "
  "         .    "
  "    ......    "
  "    .         "
  "    .         "
  "    .         "
  "    ......    "
  "         .    "
  "         .    "
  "         .    ";

static const char sig_dign[] = 
  "\x0e"
  "              "
  "    .         "
  "    .         "
  "    .         "
  "    ......    "
  "         .    "
  "         .    "
  "         .    "
  "    ......    "
  "    .         "
  "    .         "
  "    .         ";

static const char per_div[] = 
  "\x0e"
  "           .. "
  "         ..   "
  "       ..     "
  "              "
  "          ..  "
  "         .  . "
  "       ...... "
  "              "
  "       . .... "
  "              "
  "         ...  "
  "            . "
  "         ...  ";

static const char trig_pos[] = 
  "\x0e"
  "              "
  "            . "
  "        .   . "
  "       .    . "
  "  ........... "
  "  .    .      "
  "  .     .     "
  "  .           ";

static const char trig_pos_[] = 
  "\x0e"
  "            . "
  "        .. .x."
  "       .x. .x."
  "  .....x....x."
  " .xxxxxxxxxxx."
  " .x....x..... "
  " .x.   .x.    "
  " .x.    ..    "
  "  .           ";

static const char trig_neg[] = 
  "\x0e"
  "              "
  "            . "
  "      .     . "
  "       .    . "
  "  ........... "
  "  .    .      "
  "  .   .       "
  "  .           "
  "              ";

static const char trig_neg_[] = 
  "\x0e"
  "            . "
  "     ..    .x."
  "     .x.   .x."
  "  .....x....x."
  " .xxxxxxxxxxx."
  " .x....x..... "
  " .x. .x.      "
  " .x. ..       "
  "  .           ";

static const char prog_left[] = 
  "\x07"
  "......."
  ".     .";

static const char prog_right[] = 
  "\x07"
  ".     ."
  ".......";

static const char prog_in[] = 
  "\x07"
  ".     .";

static const char prog_pat[] = 
  "\x07"
  "   .   "
  "    .  "
  "     . "
  "    .  "
  "   .   "
  "  .    "
  " .     "
  "  .    ";

static const char prog_sld[] = 
  "\x07"
  " ..... "
  " ..... "
  " ..... "
  " ..... "
  " ..... "
  " ..... "
  " ..... "
  " ..... ";


static const char time_base[] = 
  "\x05"
  "..   "
  "...  "
  ".... "
  "....."
  ".... "
  "...  "
  "..   ";


static const char trig_run[] = 
  "\x0e"
  "  .........   "
  "   .......    "
  "    .....     "
  "     ...      "
  "      .       "
  "              "
  "  .........   "
  "  .........   ";

static const char trig_stop[] = 
  "\x0e"
  "    .....     "
  "   .......    "
  "  .........   "
  "  .........   "
  "  .........   "
  "   .......    "
  "    .....     "
  "              ";

static const char curx1[] = 
  "\x0e"
  ".. ..    .    "
  "...     .     "
  "  ...  ...... "
  ".. ..         ";

static const char cury1[] = 
  "\x0e"
  "..       .    "
  ".....   .     "
  "  ...  ...... "
  "..            ";

static const char curx2[] = 
  "\x0e"
  ".. ..  .    . "
  "...   .    .. "
  "  ... .   . . "
  ".. ..  ...  . ";

static const char cury2[] = 
  "\x0e"
  "..      .    ."
  ".....  .    .."
  "  ...  .   . ."
  "..      ...  .";

static const char minus[] = 
  "\x0e"
  "              "
  "      .       "
  "      .       "
  "      .       "
  "      .       "
  "              ";

static const char check[] = "check";

#endif