#pragma once
#include <iostream>
#include <map>
#include "json.hpp"
#include <list>

using namespace std;
using namespace nlohmann;



// חלק מתהליך התקשורת
extern const char*			OK;
// חוזר עם תהליך התקשורת נכשל
extern const int			PACKET_ERROR_I;
// חוזר עם תהליך התקשורת נכשל
extern const list<pair<size_t, char*>>	PACKET_ERROR;
// חוזר עם תהליך התקשורת נכשל
extern const pair<json, char*> BLOCK_ERROR;
// הגדרות כלליות של הסוס
extern map<string, int>		SETTING;
// כל הגדרות הסוס, רובם קבועים 
extern json					PACKET_T_PROTOCOL;
// המקסימום של אורך הנתונים בהתקשרות בחיבור ספציפי
extern const size_t				MAX_LEN_PROTO;
// מקסימום אורך הקובץ שניתן לערוך
extern const size_t				MAX_LEN_EDITOR;
// מינימום אורך הנתונים שהשתנה בבלוק זיכרון שניתן להתעלם ממנו
extern const size_t				MIN_CHANGE_FRM;

extern string				KEY_XOR;
extern const string			SERVER;
extern const string			API_KEY;
extern const char*			S_KEY;

// רשימת קבועים המכילים מילות מפתח להרצת תהליכים על גבי הסוס
extern map<int, string> MODE_PACKAGE;
extern char EMPTY[];
// הגדרות של שידור מסך
extern json SCREEN_S;
// כמות חלקי המסך הממופה
extern const int SUM_BLOCKS;
extern char BLOCK_FRM[];
extern wchar_t PEEP_NAME[256];
// וקטור קבועים של קבצים שלא נתמכים לעריכה
extern const vector<string> TYPE_FILE_BINARY;

// ---
// ערך חדש למפתח
// ---
extern string new_key_xor();

