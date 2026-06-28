// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Qt6 port: preference dialog resource IDs → Qt object names / property keys

#pragma once

// Note: preference_rc.h contains dialog and control IDs for the preference dialog.
// In Qt6, IDs are used primarily for testing and automation lookups.
// The actual Qt widget names are set via setObjectName() in Preference.cpp.

constexpr int IDD_PREFERENCE_BOX                             = 6000;
constexpr int IDC_BUTTON_CLOSE                               = 6001;
constexpr int IDC_LIST_DLGTITLE                              = 6002;

constexpr int IDD_PREFERENCE_SUB_TOOLBAR                     = 6010;
constexpr int IDC_TOOLBAR_GB_COLORIZATION                   = 6011;
constexpr int IDC_RADIO_COMPLETE                             = 6012;
constexpr int IDC_RADIO_PARTIAL                              = 6013;
constexpr int IDC_TOOLBAR_GB_COLORCHOICE                    = 6014;
constexpr int IDC_RADIO_DEFAULTCOLOR                        = 6015;
constexpr int IDC_RADIO_RED                                  = 6016;
constexpr int IDC_RADIO_GREEN                                = 6017;
constexpr int IDC_RADIO_BLUE                                 = 6018;
constexpr int IDC_RADIO_PURPLE                               = 6019;
constexpr int IDC_RADIO_CYAN                                 = 6020;
constexpr int IDC_RADIO_OLIVE                                = 6021;
constexpr int IDC_RADIO_YELLOW                               = 6022;
constexpr int IDC_RADIO_ACCENTCOLOR                         = 6023;
constexpr int IDC_RADIO_CUSTOMCOLOR                         = 6024;
constexpr int IDD_ACCENT_TIP_STATIC                         = 6025;

constexpr int IDD_PREFERENCE_SUB_TABBAR                     = 6040;

constexpr int IDD_PREFERENCE_SUB_GENERAL                   = 6100;

constexpr int IDC_CHECK_HIDE                                = 6102;
constexpr int IDC_RADIO_SMALLICON                           = 6103;
constexpr int IDC_RADIO_BIGICON                             = 6104;
constexpr int IDC_RADIO_STANDARD                            = 6105;

constexpr int IDC_CHECK_REDUCE                              = 6107;
constexpr int IDC_CHECK_LOCK                                = 6108;
constexpr int IDC_CHECK_DRAWINACTIVE                        = 6109;
constexpr int IDC_CHECK_ORANGE                              = 6110;
constexpr int IDC_CHECK_INACTTABDRAWBUTTON                  = 6111;
constexpr int IDC_CHECK_ENABLETABCLOSE                      = 6112;
constexpr int IDC_CHECK_DBCLICK2CLOSE                       = 6113;
constexpr int IDC_CHECK_ENABLEDOCSWITCHER                   = 6114;
constexpr int IDC_CHECK_ENABLETABPIN                        = 6115;
constexpr int IDC_CHECK_KEEPINSAMEDIR                       = 6116;
constexpr int IDC_CHECK_STYLEMRU                            = 6117;
constexpr int IDC_CHECK_TAB_HIDE                            = 6118;
constexpr int IDC_CHECK_TAB_MULTILINE                       = 6119;
constexpr int IDC_CHECK_TAB_VERTICAL                        = 6120;
constexpr int IDC_CHECK_TAB_LAST_EXIT                       = 6121;
constexpr int IDC_CHECK_HIDEMENUBAR                         = 6122;
constexpr int IDC_LOCALIZATION_GB_STATIC                   = 6123;
constexpr int IDC_COMBO_LOCALIZATION                        = 6124;
constexpr int IDC_GB_BEHAVIOR                               = 6125;
constexpr int IDC_GB_LOOKFEEL                              = 6126;

constexpr int IDC_CHECK_TAB_ALTICONS                        = 6128;
constexpr int IDC_RADIO_SMALLICON2                         = 6129;
constexpr int IDC_RADIO_BIGICON2                           = 6130;
constexpr int IDC_MENU_GB_STATIC                           = 6131;
constexpr int IDC_CHECK_HIDERIGHTSHORTCUTSOFMENUBAR         = 6132;
constexpr int IDC_STATUSBAR_GB_STATIC                       = 6133;
constexpr int IDC_CHECK_HIDESTATUSBAR                      = 6134;
constexpr int IDC_CHECK_SHOWONLYPINNEDBUTTON                = 6135;

constexpr int IDC_TABCOMPACTLABELLEN_STATIC                = 6136;
constexpr int IDC_EDIT_TABCOMPACTLABELLEN                  = 6137;
constexpr int IDC_TABCOMPACTLABELLEN_TIP_STATIC             = 6138;

constexpr int IDD_PREFERENCE_SUB_MULTIINSTANCE             = 6150;
constexpr int IDC_MULTIINST_GB_STATIC                      = 6151;
constexpr int IDC_SESSIONININST_RADIO                      = 6152;
constexpr int IDC_MULTIINST_RADIO                          = 6153;
constexpr int IDC_MONOINST_RADIO                           = 6154;
constexpr int IDD_STATIC_RESTARTNOTE                       = 6155;

constexpr int IDC_WORDCHARLIST_GB_STATIC                   = 6161;
constexpr int IDC_RADIO_WORDCHAR_DEFAULT                    = 6162;
constexpr int IDC_RADIO_WORDCHAR_CUSTOM                     = 6163;
constexpr int IDC_WORDCHAR_CUSTOM_EDIT                     = 6164;
constexpr int IDD_WORDCHAR_QUESTION_BUTTON                 = 6165;
constexpr int IDD_STATIC_WORDCHAR_WARNING                  = 6166;

constexpr int IDC_DATETIMEFORMAT_GB_STATIC                 = 6171;
constexpr int IDD_DATETIMEFORMAT_STATIC                    = 6172;
constexpr int IDC_DATETIMEFORMAT_EDIT                      = 6173;
constexpr int IDD_DATETIMEFORMAT_RESULT_STATIC             = 6174;
constexpr int IDD_DATETIMEFORMAT_REVERSEORDER_CHECK        = 6175;

constexpr int IDC_PANEL_IGNORESESSION_GB_STATIC           = 6181;
constexpr int IDD_STATIC_PANELSTATE_DESCRIPTION            = 6182;
constexpr int IDC_CHECK_CLIPBOARDHISTORY                   = 6183;
constexpr int IDC_CHECK_DOCLIST                             = 6184;
constexpr int IDC_CHECK_CHARPANEL                           = 6185;
constexpr int IDC_CHECK_FILEBROWSER                         = 6186;
constexpr int IDC_CHECK_PROJECTPANEL                        = 6187;
constexpr int IDC_CHECK_DOCMAP                              = 6188;
constexpr int IDC_CHECK_FUNCLIST                            = 6189;
constexpr int IDC_CHECK_PLUGINPANEL                         = 6190;

constexpr int IDD_PREFERENCE_SUB_EDITING                   = 6200;
constexpr int IDC_FMS_GB_STATIC                           = 6201;
constexpr int IDC_RADIO_SIMPLE                             = 6202;
constexpr int IDC_RADIO_ARROW                              = 6203;
constexpr int IDC_RADIO_CIRCLE                             = 6204;
constexpr int IDC_RADIO_BOX                                = 6205;

constexpr int IDC_CHECK_LINENUMBERMARGE                    = 6206;
constexpr int IDC_CHECK_BOOKMARKMARGE                       = 6207;

constexpr int IDC_PADDING_STATIC                           = 6208;
constexpr int IDC_PADDINGLEFT_STATIC                      = 6209;
constexpr int IDC_PADDINGRIGHT_STATIC                      = 6210;

constexpr int IDC_VES_GB_STATIC                           = 6211;
constexpr int IDC_DISTRACTIONFREE_STATIC                   = 6212;
constexpr int IDC_CHECK_EDGEBGMODE                        = 6213;
constexpr int IDC_CHECK_LINECUTCOPYWITHOUTSELECTION         = 6214;
constexpr int IDC_CHECK_SMOOTHFONT                          = 6215;

constexpr int IDC_CARETSETTING_STATIC                      = 6216;
constexpr int IDC_WIDTH_STATIC                             = 6217;
constexpr int IDC_WIDTH_COMBO                              = 6218;
constexpr int IDC_BLINKRATE_STATIC                         = 6219;
constexpr int IDC_CARETBLINKRATE_SLIDER                    = 6220;
constexpr int IDC_CARETBLINKRATE_F_STATIC                  = 6221;
constexpr int IDC_CARETBLINKRATE_S_STATIC                  = 6222;
constexpr int IDC_CHECK_CHANGHISTORYMARGIN                 = 6223;
constexpr int IDC_DISTRACTIONFREE_SLIDER                   = 6224;
constexpr int IDC_CHECK_SELECTEDTEXTSINGLECOLOR             = 6225;

constexpr int IDC_RADIO_FOLDMARGENONE                      = 6226;

constexpr int IDC_LW_GB_STATIC                            = 6227;
constexpr int IDC_RADIO_LWDEF                             = 6228;
constexpr int IDC_RADIO_LWALIGN                            = 6229;
constexpr int IDC_RADIO_LWINDENT                           = 6230;

constexpr int IDC_BORDERWIDTH_STATIC                      = 6231;
constexpr int IDC_BORDERWIDTHVAL_STATIC                   = 6232;
constexpr int IDC_BORDERWIDTH_SLIDER                       = 6233;
constexpr int IDC_CHECK_DISABLEADVANCEDSCROLL              = 6234;
constexpr int IDC_CHECK_NOEDGE                             = 6235;
constexpr int IDC_CHECK_SCROLLBEYONDLASTLINE                = 6236;

constexpr int IDC_STATIC_MULTILNMODE_TIP                  = 6237;
constexpr int IDC_COLUMNPOS_EDIT                          = 6238;
constexpr int IDC_CHECK_RIGHTCLICKKEEPSSELECTION           = 6239;
constexpr int IDC_PADDINGLEFT_SLIDER                      = 6240;
constexpr int IDC_PADDINGRIGHT_SLIDER                      = 6241;
constexpr int IDC_PADDINGLEFTVAL_STATIC                    = 6242;
constexpr int IDC_PADDINGRIGHTVAL_STATIC                  = 6243;
constexpr int IDC_DISTRACTIONFREEVAL_STATIC               = 6244;

constexpr int IDC_CHECK_VIRTUALSPACE                       = 6245;
constexpr int IDC_CHECK_FOLDINGTOGGLE                      = 6246;

constexpr int IDC_GB_STATIC_CRLF                          = 6247;
constexpr int IDC_RADIO_ROUNDCORNER_CRLF                  = 6248;
constexpr int IDC_RADIO_PLAINTEXT_CRLF                    = 6249;
constexpr int IDC_CHECK_WITHCUSTOMCOLOR_CRLF              = 6250;
constexpr int IDC_BUTTON_LAUNCHSTYLECONF_CRLF              = 6251;

constexpr int IDC_GB_STATIC_NPC                           = 6252;
constexpr int IDC_BUTTON_NPC_NOTE                          = 6253;
constexpr int IDC_RADIO_NPC_ABBREVIATION                   = 6254;
constexpr int IDC_RADIO_NPC_CODEPOINT                     = 6255;
constexpr int IDC_CHECK_NPC_COLOR                         = 6256;
constexpr int IDC_BUTTON_NPC_LAUNCHSTYLECONF              = 6257;
constexpr int IDC_CHECK_NPC_INCLUDECCUNIEOL                = 6258;
constexpr int IDC_CHECK_NPC_NOINPUTC0                      = 6259;
constexpr int IDC_STATIC_NPC_APPEARANCE                    = 6260;

constexpr int IDD_PREFERENCE_SUB_DELIMITER                = 6250;
constexpr int IDC_DELIMITERSETTINGS_GB_STATIC             = 6251;
constexpr int IDD_STATIC_OPENDELIMITER                    = 6252;
constexpr int IDC_EDIT_OPENDELIMITER                      = 6253;
constexpr int IDC_EDIT_CLOSEDELIMITER                     = 6254;
constexpr int IDD_STATIC_CLOSEDELIMITER                   = 6255;
constexpr int IDD_SEVERALLINEMODEON_CHECK                 = 6256;
constexpr int IDD_STATIC_BLABLA                           = 6257;
constexpr int IDD_STATIC_BLABLA2NDLINE                    = 6258;

constexpr int IDD_PREFERENCE_SUB_CLOUD_LINK               = 6260;
constexpr int IDC_SETTINGSONCLOUD_WARNING_STATIC          = 6261;
constexpr int IDC_SETTINGSONCLOUD_GB_STATIC               = 6262;
constexpr int IDC_NOCLOUD_RADIO                           = 6263;
constexpr int IDC_URISCHEMES_STATIC                       = 6264;
constexpr int IDC_URISCHEMES_EDIT                         = 6265;
constexpr int IDC_CHECK_DISABLESELECTEDTEXTDRAGDROP        = 6266;
constexpr int IDC_WITHCLOUD_RADIO                         = 6267;
constexpr int IDC_CLOUDPATH_EDIT                          = 6268;
constexpr int IDD_CLOUDPATH_BROWSE_BUTTON                 = 6269;

constexpr int IDD_PREFERENCE_SUB_SEARCHENGINE             = 6270;
constexpr int IDC_SEARCHENGINES_GB_STATIC                 = 6271;
constexpr int IDC_SEARCHENGINE_DUCKDUCKGO_RADIO           = 6272;
constexpr int IDC_SEARCHENGINE_GOOGLE_RADIO               = 6273;
constexpr int IDC_SEARCHENGINE_YAHOO_RADIO               = 6275;
constexpr int IDC_SEARCHENGINE_CUSTOM_RADIO               = 6276;
constexpr int IDC_SEARCHENGINE_EDIT                       = 6277;
constexpr int IDD_SEARCHENGINE_NOTE_STATIC                = 6278;
constexpr int IDC_SEARCHENGINE_STACKOVERFLOW_RADIO        = 6279;

constexpr int IDD_PREFERENCE_SUB_MARGING_BORDER_EDGE      = 6290;
constexpr int IDC_LINENUMBERMARGE_GB_STATIC               = 6291;
constexpr int IDC_RADIO_DYNAMIC                           = 6292;
constexpr int IDC_RADIO_CONSTANT                          = 6293;
constexpr int IDC_BUTTON_VES_TIP                          = 6294;
constexpr int IDC_GB_CHANGHISTORY                         = 6295;
constexpr int IDC_CHECK_CHANGHISTORYINDICATOR             = 6296;

constexpr int IDD_PREFERENCE_SUB_MISC                     = 6300;
constexpr int IDC_TABSETTING_GB_STATIC                   = 6301;
constexpr int IDC_RADIO_REPLACEBYSPACE                   = 6302;
constexpr int IDC_TABSIZE_STATIC                         = 6303;
constexpr int IDC_HISTORY_GB_STATIC                      = 6304;
constexpr int IDC_CHECK_DONTCHECKHISTORY                  = 6305;
constexpr int IDC_MAXNBFILE_STATIC                       = 6306;
constexpr int IDC_COMBO_SYSTRAY_ACTION_CHOICE             = 6307;
constexpr int IDC_SYSTRAY_STATIC                          = 6308;
constexpr int IDC_CHECK_REMEMBERSESSION                   = 6309;
constexpr int IDC_INDENTUSING_STATIC                     = 6310;
constexpr int IDC_RADIO_USINGTAB                          = 6311;
constexpr int IDC_FILEAUTODETECTION_STATIC                = 6312;
constexpr int IDC_CHECK_UPDATESILENTLY                    = 6313;
constexpr int IDC_RADIO_BKNONE                            = 6315;
constexpr int IDC_RADIO_BKSIMPLE                          = 6316;
constexpr int IDC_RADIO_BKVERBOSE                         = 6317;
constexpr int IDC_CLICKABLELINK_STATIC                   = 6318;
constexpr int IDC_CHECK_CLICKABLELINK_ENABLE              = 6319;
constexpr int IDC_CHECK_CLICKABLELINK_NOUNDERLINE         = 6320;
constexpr int IDC_EDIT_SESSIONFILEEXT                     = 6321;
constexpr int IDC_SESSIONFILEEXT_STATIC                   = 6322;
constexpr int IDC_DOCUMENTSWITCHER_STATIC                 = 6324;
constexpr int IDC_CHECK_UPDATEGOTOEOF                     = 6325;
constexpr int IDC_CHECK_ENABLSMARTHILITE                  = 6326;
constexpr int IDC_CHECK_ENABLTAGSMATCHHILITE              = 6327;
constexpr int IDC_CHECK_ENABLTAGATTRHILITE                = 6328;
constexpr int IDC_TAGMATCHEDHILITE_STATIC                 = 6329;
constexpr int IDC_CHECK_HIGHLITENONEHTMLZONE              = 6330;
constexpr int IDC_CHECK_SHORTTITLE                        = 6331;
constexpr int IDC_CHECK_SMARTHILITECASESENSITIVE          = 6332;
constexpr int IDC_SMARTHILITING_STATIC                   = 6333;
constexpr int IDC_CHECK_DETECTENCODING                    = 6334;
constexpr int IDC_CHECK_BACKSLASHISESCAPECHARACTERFORSQL  = 6335;
constexpr int IDC_EDIT_WORKSPACEFILEEXT                   = 6336;
constexpr int IDC_WORKSPACEFILEEXT_STATIC                 = 6337;
constexpr int IDC_CHECK_SMARTHILITEWHOLEWORDONLY          = 6338;
constexpr int IDC_CHECK_SMARTHILITEUSEFINDSETTINGS       = 6339;
constexpr int IDC_CHECK_SMARTHILITEANOTHERRVIEW           = 6340;

constexpr int IDC_DOCUMENTPEEK_STATIC                    = 6344;
constexpr int IDC_CHECK_ENABLEDOCPEEKER                   = 6345;
constexpr int IDC_CHECK_ENABLEDOCPEEKONMAP                = 6346;
constexpr int IDC_COMBO_FILEUPDATECHOICE                  = 6347;
constexpr int IDC_CHECK_DIRECTWRITE_ENABLE                = 6349;
constexpr int IDC_CHECK_CLICKABLELINK_FULLBOXMODE         = 6350;
constexpr int IDC_MARKALL_STATIC                          = 6351;
constexpr int IDC_CHECK_MARKALLCASESENSITIVE              = 6352;
constexpr int IDC_CHECK_MARKALLWHOLEWORDONLY              = 6353;
constexpr int IDC_SMARTHILITEMATCHING_STATIC              = 6354;
constexpr int IDC_CHECK_MUTE_SOUNDS                       = 6360;
constexpr int IDC_CHECK_SAVEALLCONFIRM                    = 6361;

constexpr int IDC_COMBO_SC_TECHNOLOGY_CHOICE              = 6362;
constexpr int IDC_SC_TECHNOLOGY_STATIC                    = 6363;
constexpr int IDC_COMBO_AUTOUPDATE                        = 6364;
constexpr int IDC_AUTOUPDATE_STATIC                       = 6365;

constexpr int IDD_PREFERENCE_SUB_NEWDOCUMENT              = 6400;
constexpr int IDC_FORMAT_GB_STATIC                        = 6401;
constexpr int IDC_RADIO_F_WIN                             = 6402;
constexpr int IDC_RADIO_F_UNIX                            = 6403;
constexpr int IDC_RADIO_F_MAC                             = 6404;
constexpr int IDC_ENCODING_STATIC                         = 6405;
constexpr int IDC_RADIO_ANSI                              = 6406;
constexpr int IDC_RADIO_UTF8_NO_BOM                      = 6407;
constexpr int IDC_RADIO_UTF8                              = 6408;
constexpr int IDC_RADIO_UTF16BIG                          = 6409;
constexpr int IDC_RADIO_UTF16SMALL                        = 6410;
constexpr int IDC_DEFAULTLANG_STATIC                      = 6411;
constexpr int IDC_COMBO_DEFAULTLANG                       = 6412;
constexpr int IDC_OPENSAVEDIR_GR_STATIC                   = 6413;
constexpr int IDC_OPENSAVEDIR_FOLLOWCURRENT_RADIO         = 6414;
constexpr int IDC_OPENSAVEDIR_REMEMBERLAST_RADIO          = 6415;
constexpr int IDC_OPENSAVEDIR_ALWAYSON_RADIO              = 6416;
constexpr int IDC_OPENSAVEDIR_ALWAYSON_EDIT               = 6417;
constexpr int IDD_OPENSAVEDIR_ALWAYSON_BROWSE_BUTTON      = 6418;
constexpr int IDC_NEWDOCUMENT_GR_STATIC                   = 6419;
constexpr int IDC_CHECK_OPENANSIASUTF8                    = 6420;
constexpr int IDC_RADIO_OTHERCP                           = 6421;
constexpr int IDC_COMBO_OTHERCP                           = 6422;
constexpr int IDC_GP_STATIC_RECENTFILES                   = 6423;
constexpr int IDC_CHECK_INSUBMENU                         = 6424;
constexpr int IDC_RADIO_ONLYFILENAME                      = 6425;
constexpr int IDC_RADIO_FULLFILENAMEPATH                  = 6426;
constexpr int IDC_RADIO_CUSTOMIZELENTH                    = 6427;

constexpr int IDC_DISPLAY_STATIC                          = 6429;
constexpr int IDC_OPENSAVEDIR_CHECK_DROPFOLDEROPENFILES   = 6431;
constexpr int IDC_CHECK_ADDNEWDOCONSTARTUP                = 6432;
constexpr int IDC_CHECK_USECONTENTASTABNAME               = 6433;

constexpr int IDD_PREFERENCE_SUB_DEFAULTDIRECTORY         = 6450;
constexpr int IDD_PREFERENCE_SUB_RECENTFILESHISTORY       = 6460;
constexpr int IDC_EDIT_MAXNBFILEVAL                       = 6461;
constexpr int IDC_MAXNBFILE_RANGE_STATIC                  = 6462;
constexpr int IDC_EDIT_CUSTOMIZELENGTHVAL                 = 6463;
constexpr int IDC_CUSTOMIZELENGTH_RANGE_STATIC            = 6464;

constexpr int IDD_PREFERENCE_SUB_LANGUAGE                 = 6500;
constexpr int IDC_LIST_ENABLEDLANG                        = 6501;
constexpr int IDC_LIST_DISABLEDLANG                       = 6502;
constexpr int IDC_BUTTON_REMOVE                           = 6503;
constexpr int IDC_BUTTON_RESTORE                          = 6504;
constexpr int IDC_ENABLEDITEMS_STATIC                    = 6505;
constexpr int IDC_DISABLEDITEMS_STATIC                    = 6506;
constexpr int IDC_CHECK_LANGMENUCOMPACT                   = 6507;
constexpr int IDC_CHECK_LANGMENU_GR_STATIC                = 6508;
constexpr int IDC_LIST_TABSETTNG                         = 6509;
constexpr int IDC_CHECK_DEFAULTTABVALUE                   = 6510;
constexpr int IDC_GR_TABVALUE_STATIC                     = 6511;
constexpr int IDC_CHECK_BACKSPACEUNINDENT                 = 6512;
constexpr int IDC_EDIT_TABSIZEVAL                        = 6513;

constexpr int IDD_PREFERENCE_SUB_EDITING2                 = 6520;
constexpr int IDC_GB_STATIC_MULTIEDITING                  = 6521;
constexpr int IDC_CHECK_MULTISELECTION                    = 6522;
constexpr int IDC_CHECK_COLUMN2MULTIEDITING               = 6523;

constexpr int IDD_PREFERENCE_SUB_HIGHLIGHTING             = 6550;

constexpr int IDD_PREFERENCE_SUB_PRINT                    = 6600;
constexpr int IDC_CHECK_PRINTLINENUM                     = 6601;
constexpr int IDC_COLOUROPT_STATIC                        = 6602;
constexpr int IDC_RADIO_WYSIWYG                           = 6603;
constexpr int IDC_RADIO_INVERT                           = 6604;
constexpr int IDC_RADIO_BW                                = 6605;
constexpr int IDC_RADIO_NOBG                              = 6606;
constexpr int IDC_MARGESETTINGS_STATIC                   = 6607;
constexpr int IDC_EDIT_ML                                = 6608;
constexpr int IDC_EDIT_MT                                = 6609;
constexpr int IDC_EDIT_MR                                = 6610;
constexpr int IDC_EDIT_MB                                = 6611;
constexpr int IDC_ML_STATIC                              = 6612;
constexpr int IDC_MT_STATIC                              = 6613;
constexpr int IDC_MR_STATIC                              = 6614;
constexpr int IDC_MB_STATIC                              = 6615;

constexpr int IDC_CURRENTLINEMARK_STATIC                  = 6651;
constexpr int IDC_RADIO_CLM_NONE                         = 6652;
constexpr int IDC_RADIO_CLM_HILITE                       = 6653;
constexpr int IDC_RADIO_CLM_FRAME                        = 6654;
constexpr int IDC_CARETLINEFRAME_WIDTH_STATIC             = 6655;
constexpr int IDC_CARETLINEFRAME_WIDTH_SLIDER            = 6656;
constexpr int IDC_CARETLINEFRAME_WIDTH_DISPLAY            = 6657;

constexpr int IDC_EDIT_HLEFT                             = 6701;
constexpr int IDC_EDIT_HMIDDLE                           = 6702;
constexpr int IDC_EDIT_HRIGHT                            = 6703;
constexpr int IDC_COMBO_HFONTNAME                        = 6704;
constexpr int IDC_COMBO_HFONTSIZE                        = 6705;
constexpr int IDC_CHECK_HBOLD                             = 6706;
constexpr int IDC_CHECK_HITALIC                           = 6707;
constexpr int IDC_HGB_STATIC                             = 6708;
constexpr int IDC_HL_STATIC                              = 6709;
constexpr int IDC_HM_STATIC                              = 6710;
constexpr int IDC_HR_STATIC                              = 6711;
constexpr int IDC_EDIT_FLEFT                             = 6712;
constexpr int IDC_EDIT_FMIDDLE                           = 6713;
constexpr int IDC_EDIT_FRIGHT                            = 6714;
constexpr int IDC_COMBO_FFONTNAME                        = 6715;
constexpr int IDC_COMBO_FFONTSIZE                        = 6716;
constexpr int IDC_CHECK_FBOLD                             = 6717;
constexpr int IDC_CHECK_FITALIC                           = 6718;
constexpr int IDC_FGB_STATIC                             = 6719;
constexpr int IDC_FL_STATIC                              = 6720;
constexpr int IDC_FM_STATIC                              = 6721;
constexpr int IDC_FR_STATIC                              = 6722;
constexpr int IDC_BUTTON_ADDVAR                          = 6723;
constexpr int IDC_COMBO_VARLIST                          = 6724;
constexpr int IDC_VAR_STATIC                             = 6725;
constexpr int IDC_EDIT_VIEWPANEL                         = 6726;
constexpr int IDC_WHICHPART_STATIC                       = 6727;
constexpr int IDC_HEADERFPPTER_GR_STATIC                 = 6728;

constexpr int IDD_PREFERENCE_SUB_BACKUP                  = 6800;
constexpr int IDC_BACKUPDIR_GRP_STATIC                   = 6801;
constexpr int IDC_BACKUPDIR_USERCUSTOMDIR_GRPSTATIC      = 6802;
constexpr int IDD_BACKUPDIR_STATIC                       = 6803;
constexpr int IDC_BACKUPDIR_CHECK                        = 6804;
constexpr int IDC_BACKUPDIR_EDIT                        = 6805;
constexpr int IDD_BACKUPDIR_BROWSE_BUTTON               = 6806;
constexpr int IDD_AUTOC_GRPSTATIC                       = 6807;
constexpr int IDD_AUTOC_ENABLECHECK                     = 6808;
constexpr int IDD_AUTOC_FUNCRADIO                       = 6809;
constexpr int IDD_AUTOC_WORDRADIO                       = 6810;
constexpr int IDD_AUTOC_STATIC_FROM                     = 6811;
constexpr int IDD_AUTOC_STATIC_N                        = 6812;
constexpr int IDD_AUTOC_STATIC_CHAR                     = 6813;
constexpr int IDD_FUNC_CHECK                            = 6815;
constexpr int IDD_AUTOC_BOTHRADIO                       = 6816;
constexpr int IDC_BACKUPDIR_RESTORESESSION_GRP_STATIC   = 6817;
constexpr int IDC_BACKUPDIR_RESTORESESSION_CHECK        = 6818;
constexpr int IDD_BACKUPDIR_RESTORESESSION_STATIC1      = 6819;
constexpr int IDC_BACKUPDIR_RESTORESESSION_EDIT         = 6820;
constexpr int IDD_BACKUPDIR_RESTORESESSION_STATIC2      = 6821;
constexpr int IDD_BACKUPDIR_RESTORESESSION_PATHLABEL_STATIC = 6822;
constexpr int IDD_BACKUPDIR_RESTORESESSION_PATH_EDIT    = 6823;
constexpr int IDD_AUTOC_IGNORENUMBERS                   = 6824;
constexpr int IDC_CHECK_KEEPABSENTFILESINSESSION        = 6825;

constexpr int IDD_PREFERENCE_SUB_AUTOCOMPLETION         = 6850;
constexpr int IDD_AUTOCINSERT_GRPSTATIC                 = 6851;
constexpr int IDD_AUTOCPARENTHESES_CHECK                = 6852;
constexpr int IDD_AUTOCBRACKET_CHECK                    = 6853;
constexpr int IDD_AUTOCCURLYBRACKET_CHECK               = 6854;
constexpr int IDD_AUTOC_DOUBLEQUOTESCHECK               = 6855;
constexpr int IDD_AUTOC_QUOTESCHECK                     = 6856;
constexpr int IDD_AUTOCTAG_CHECK                        = 6857;
constexpr int IDC_MACHEDPAIROPEN_STATIC                  = 6858;
constexpr int IDC_MACHEDPAIRCLOSE_STATIC                = 6859;
constexpr int IDC_MACHEDPAIR_STATIC1                   = 6860;
constexpr int IDC_MACHEDPAIROPEN_EDIT1                  = 6861;
constexpr int IDC_MACHEDPAIRCLOSE_EDIT1                = 6862;
constexpr int IDC_MACHEDPAIR_STATIC2                   = 6863;
constexpr int IDC_MACHEDPAIROPEN_EDIT2                  = 6864;
constexpr int IDC_MACHEDPAIRCLOSE_EDIT2                = 6865;
constexpr int IDC_MACHEDPAIR_STATIC3                   = 6866;
constexpr int IDC_MACHEDPAIROPEN_EDIT3                  = 6867;
constexpr int IDC_MACHEDPAIRCLOSE_EDIT3                = 6868;
constexpr int IDD_AUTOC_USEKEY_GRP_STATIC               = 6869;
constexpr int IDD_AUTOC_USETAB                           = 6870;
constexpr int IDD_AUTOC_USEENTER                        = 6871;
constexpr int IDD_AUTOC_BRIEF_CHECK                     = 6872;
constexpr int IDC_AUTOC_CHAR_SLIDER                     = 6873;
constexpr int IDD_AUTOC_SLIDER_MIN_STATIC               = 6874;
constexpr int IDD_AUTOC_SLIDER_MAX_STATIC               = 6875;

constexpr int IDD_PREFERENCE_SUB_SEARCHING              = 6900;

constexpr int IDC_CHECK_MONOSPACEDFONT_FINDDLG          = 6902;
constexpr int IDC_CHECK_FINDDLG_ALWAYS_VISIBLE          = 6903;
constexpr int IDC_CHECK_CONFIRMREPLOPENDOCS              = 6904;
constexpr int IDC_CHECK_REPLACEANDSTOP                  = 6905;
constexpr int IDC_CHECK_SHOWONCEPERFOUNDLINE            = 6906;
constexpr int IDD_FILL_FIND_FIELD_GRP_STATIC            = 6907;
constexpr int IDC_CHECK_FILL_FIND_FIELD_WITH_SELECTED   = 6908;
constexpr int IDC_CHECK_FILL_FIND_FIELD_SELECT_CARET    = 6909;
constexpr int IDC_INSELECTION_THRESHOLD_STATIC          = 6910;
constexpr int IDC_INSELECTION_THRESHOLD_EDIT            = 6911;
constexpr int IDC_INSELECTION_THRESH_QUESTION_BUTTON    = 6912;
constexpr int IDC_CHECK_FILL_DIR_FIELD_FROM_ACTIVE_DOC  = 6913;
constexpr int IDC_FILLFINDWHAT_THRESH_QUESTION_BUTTON   = 6914;
constexpr int IDC_FILLFINDWHAT_THRESHOLD_EDIT           = 6915;
constexpr int IDC_FILLFINDWHAT_THRESHOLD_STATIC         = 6916;
constexpr int IDC_CHECK_FIF_IGNOREOPENEDFILES           = 6917;

constexpr int IDD_PREFERENCE_SUB_DARKMODE               = 7100;

constexpr int IDC_RADIO_DARKMODE_BLACK                  = 7102;
constexpr int IDC_RADIO_DARKMODE_RED                    = 7103;
constexpr int IDC_RADIO_DARKMODE_GREEN                  = 7104;
constexpr int IDC_RADIO_DARKMODE_BLUE                   = 7105;
constexpr int IDC_RADIO_DARKMODE_PURPLE                 = 7107;
constexpr int IDC_RADIO_DARKMODE_CYAN                   = 7108;
constexpr int IDC_RADIO_DARKMODE_OLIVE                  = 7109;

constexpr int IDC_RADIO_DARKMODE_CUSTOMIZED             = 7115;
constexpr int IDD_CUSTOMIZED_COLOR1_STATIC              = 7116;
constexpr int IDD_CUSTOMIZED_COLOR2_STATIC              = 7117;
constexpr int IDD_CUSTOMIZED_COLOR3_STATIC              = 7118;
constexpr int IDD_CUSTOMIZED_COLOR4_STATIC              = 7119;
constexpr int IDD_CUSTOMIZED_COLOR5_STATIC              = 7120;
constexpr int IDD_CUSTOMIZED_COLOR6_STATIC              = 7121;
constexpr int IDD_CUSTOMIZED_COLOR7_STATIC              = 7122;
constexpr int IDD_CUSTOMIZED_COLOR8_STATIC              = 7123;
constexpr int IDD_CUSTOMIZED_COLOR9_STATIC              = 7124;
constexpr int IDD_CUSTOMIZED_COLOR10_STATIC             = 7125;
constexpr int IDD_CUSTOMIZED_COLOR11_STATIC             = 7126;
constexpr int IDD_CUSTOMIZED_COLOR12_STATIC            = 7127;

constexpr int IDD_CUSTOMIZED_RESET_BUTTON               = 7130;
constexpr int IDC_RADIO_DARKMODE_LIGHTMODE              = 7131;
constexpr int IDC_RADIO_DARKMODE_DARKMODE               = 7132;
constexpr int IDC_RADIO_DARKMODE_FOLLOWWINDOWS         = 7133;

constexpr int IDC_DARKMODE_TONES_GB_STATIC              = 7135;
constexpr int IDD_DROPDOWN_RESET_RED                    = 7136;
constexpr int IDD_DROPDOWN_RESET_GREEN                  = 7137;
constexpr int IDD_DROPDOWN_RESET_BLUE                   = 7138;
constexpr int IDD_DROPDOWN_RESET_PURPLE                 = 7139;
constexpr int IDD_DROPDOWN_RESET_CYAN                   = 7140;
constexpr int IDD_DROPDOWN_RESET_OLIVE                  = 7141;

constexpr int IDD_PREFERENCE_SUB_PERFORMANCE           = 7140;
constexpr int IDC_GROUPSTATIC_PERFORMANCE_RESTRICTION  = 7141;
constexpr int IDD_PERFORMANCE_TIP_QUESTION_BUTTON       = 7142;
constexpr int IDC_CHECK_PERFORMANCE_ENABLE              = 7143;
constexpr int IDC_STATIC_PERFORMANCE_FILESIZE           = 7144;
constexpr int IDC_EDIT_PERFORMANCE_FILESIZE             = 7145;
constexpr int IDC_STATIC_PERFORMANCE_MB                 = 7146;
constexpr int IDC_CHECK_PERFORMANCE_ALLOWBRACEMATCH    = 7147;
constexpr int IDC_CHECK_PERFORMANCE_ALLOWAUTOCOMPLETION = 7148;
constexpr int IDC_CHECK_PERFORMANCE_ALLOWSMARTHILITE   = 7149;
constexpr int IDC_CHECK_PERFORMANCE_DEACTIVATEWORDWRAP  = 7150;
constexpr int IDC_CHECK_PERFORMANCE_ALLOWCLICKABLELINK = 7151;
constexpr int IDC_CHECK_PERFORMANCE_SUPPRESS2GBWARNING = 7152;

constexpr int IDD_PREFERENCE_SUB_INDENTATION            = 7160;
constexpr int IDC_GROUPSTATIC_AUTOINDENT               = 7161;
constexpr int IDC_RADIO_AUTOINDENT_NONE                = 7162;
constexpr int IDC_RADIO_AUTOINDENT_BASIC               = 7163;
constexpr int IDC_RADIO_AUTOINDENT_ADVANCED            = 7164;
