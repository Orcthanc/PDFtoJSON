#include "Reader.h"
#include "Class.h"

#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFIndirectObjectReference.h>

#include <map>
#include <algorithm>

std::map<std::string, EPDFFormObjectTValuesType> stoType = {
	#define VALUE( name ) { #name, e##name },
	#include "pdfFormObjectTValues.inl"
};

std::vector<ObjectIDType> already_parsed;

static std::string sanitize( std::string string ){
	std::replace( string.begin(), string.end(), ' ', '_' );
	return string;
}

bool readObject( Character& character, PDFDictionary& object, PDFParser& parser, std::string prefix ){
	if( object.Exists( "P" )){
		PDFObject *obj = object.QueryDirectObject( "P" );
		if( obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference ){
			if( std::find( already_parsed.begin(), already_parsed.end(), ((PDFIndirectObjectReference*)obj)->mObjectID ) == already_parsed.end() ){
				already_parsed.push_back( ((PDFIndirectObjectReference*)obj)->mObjectID );
				obj = parser.ParseNewObject( ((PDFIndirectObjectReference*)obj)->mObjectID );

				std::string newprefix;

				if( object.Exists( "T" )){
					newprefix = std::string( ((PDFLiteralString*)object.QueryDirectObject( "T" ))->GetValue() + "__" );
				}else{
					newprefix = "";
				}

				readObject( character, *(PDFDictionary*)obj, parser, prefix + newprefix );
			}
		}else{
			std::string newprefix;

			if( object.Exists( "T" )){
				newprefix = std::string( ((PDFLiteralString*)object.QueryDirectObject( "T" ))->GetValue() + "__" );
			}else{
				newprefix = "";
			}

			readObject( character, *(PDFDictionary*)obj, parser, prefix + newprefix );
		}
	}

	if( object.Exists( "Annots" )){
		PDFObject *obj = object.QueryDirectObject( "Annots" );
		if( obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference ){
			if( std::find( already_parsed.begin(), already_parsed.end(), ((PDFIndirectObjectReference*)obj)->mObjectID ) == already_parsed.end() ){
				already_parsed.push_back( ((PDFIndirectObjectReference*)obj)->mObjectID );
				obj = parser.ParseNewObject( ((PDFIndirectObjectReference*)obj)->mObjectID );

				std::string newprefix;

				if( object.Exists( "T" )){
					newprefix = std::string( ((PDFLiteralString*)object.QueryDirectObject( "T" ))->GetValue() + "__" );
				}else{
					newprefix = "";
				}

				readArr( character, *(PDFArray*)obj, parser, prefix + newprefix );
			}
		}else{
			std::string newprefix;

			if( object.Exists( "T" )){
				newprefix = std::string( ((PDFLiteralString*)object.QueryDirectObject( "T" ))->GetValue() + "__" );
			}else{
				newprefix = "";
			}

			readArr( character, *(PDFArray*)obj, parser, prefix + newprefix );
		}
	}


	if( !object.Exists( "T" )){
		if( object.Exists( "V" )){
			printf( "Found weird object:\t\t\t%s\n", ((PDFLiteralString*)object.QueryDirectObject( "V" ))->GetValue().c_str() );
		}
		return false;
	}


	auto s_key = sanitize( ((PDFLiteralString*)object.QueryDirectObject( "T" ))->GetValue() );

	if( object.Exists( "Kids" )){
		readArr( character, *(PDFArray*)object.QueryDirectObject( "Kids" ), parser, prefix + s_key + "__" );
		
	}
	
	if( !object.Exists( "V" ) ){
//		printf( "Dafaq? Probably found an incorrect form.\n" );
		return false;
	}

	std::string value = std::string(((PDFLiteralString*)object.QueryDirectObject( "V" ))->GetValue());

	auto key = stoType.find( prefix + s_key );
	if( 	//key == stoType.end() 
			true){
		printf( "Keyword %s%s not found:\t\t%s\n", prefix.c_str(), s_key.c_str(), sanitize( value ).c_str() );
		return false;
	}
	switch( key->second ){
	//classname
	case eclass:
		if( character.classes.size() == 0 ){
			printf( "Created class\n" );
			character.classes.push_back( new DND_Class() );
		}
		character.classes[0]->name = value;
		return true;
	//spellslot used?	Either "Yes" or "Off"
	case espell1__4:
		return false;
	case espell1__3:
		return false;
	case espell1__2:
		return false;
	case espell1__1:
		return false;
	case espell2__3:
		return false;
	case espell2__2:
		return false;
	case espell2__1:
		return false;
	case espell3__2:
		return false;
	case espell3__3:
		return false;
	case espell3__1:
		return false;
	//Spellparams "Yes" or "Off"
	case espell3__13__conc:
		return false;
	case espell3__13__S:
		return false;
	case espell3__12__S:
		return false;
	case espell3__12__M:
		return false;
	case espell3__11__V:
		return false;
	case espell3__10__S:
		return false;
	case espell3__10__V:
		return false;
	case espell3__9__conc:
		return false;
	case espell3__8__S:
		return false;
	case espell3__7__conc:
		return false;
	case espell3__7__M:
		return false;
	case espell3__6__V:
		return false;
	case espell3__6__S:
		return false;
	case espell3__5__conc:
		return false;
	case espell3__5__V:
		return false;
	case espell3__4__prep:
		return false;
	case espell4__1:
		return false;
	case espell4__2:
		return false;
	case espell4__3:
		return false;
	case espell5__1:
		return false;
	case espell5__2:
		return false;
	//Spellslot amount string
	case eSpell_slots_2:
		return false;
	case eSpell_slots_4:
		return false;
	case eSpell_slots_3:
		return false;
	case eSpell_slots_1:
		return false;
	//Cantrip names on spellcasting sheet string
	case eCantrip1:
		return false;
	case eCantrip2:
		return false;
	case eCantrip3:
		return false;
	case eCantrip4:
		return false;
	case eCantrip5:
		return false;
	case eCantrip6:
		return false;
	case eCantrip7:
		return false;
	case eCantrip8:
		return false;
	//More spells string
	case eSpell1__13:
		return false;
	case eSpell1__2:
		return false;
	case eSpell1__3:
		return false;
	case eSpell1__4:
		return false;
	case eSpell1__5:
		return false;
	case eSpell1__6:
		return false;
	case eSpell1__7:
		return false;
	case eSpell1__8:
		return false;
	case eSpell1__9:
		return false;
	case eSpell1__10:
		return false;
	case eSpell1__11:
		return false;
	case eSpell1__12:
		return false;
	case eSpell1__1:
		return false;
	case eSpell2__1:
		return false;
	case eSpell2__2:
		return false;
	case eSpell2__3:
		return false;
	case eSpell2__4:
		return false;
	case eSpell2__5:
		return false;
	case eSpell2__6:
		return false;
	case eSpell2__7:
		return false;
	case eSpell2__8:
		return false;
	case eSpell2__9:
		return false;
	case eSpell2__10:
		return false;
	case eSpell2__11:
		return false;
	case eSpell2__12:
		return false;
	case eSpell2__13:
		return false;
	case eSpell3__1:
		return false;
	case eSpell3__2:
		return false;
	case eSpell3__3:
		return false;
	case eSpell3__4:
		return false;
	case eSpell3__5:
		return false;
	case eSpell3__6:
		return false;
	case eSpell3__7:
		return false;
	case eSpell3__8:
		return false;
	case eSpell3__9:
		return false;
	case eSpell3__10:
		return false;
	case eSpell3__11:
		return false;
	case eSpell3__12:
		return false;
	case eSpell3__13:
		return false;
	case eSpell4__1:
		return false;
	case eSpell4__2:
		return false;
	case eSpell4__3:
		return false;
	case eSpell4__4:
		return false;
	case eSpell4__5:
		return false;
	case eSpell4__6:
		return false;
	case eSpell4__7:
		return false;
	case eSpell4__8:
		return false;
	case eSpell4__9:
		return false;
	case eSpell4__10:
		return false;
	case eSpell4__11:
		return false;
	case eSpell4__12:
		return false;
	case eSpell4__13:
		return false;
	case eSpell5__1:
		return false;
	case eSpell5__2:
		return false;
	case eSpell5__3:
		return false;
	case eSpell5__4:
		return false;
	case eSpell5__5:
		return false;
	case eSpell5__6:
		return false;
	case eSpell5__7:
		return false;
	case eSpell5__8:
		return false;
	case eSpell5__9:
		return false;
	//Cantrip params "Yes" or "Off"
	case ecan__1__M:
		return false;
	case ecan__conc__1:
		return false;
	case ecan__conc__4:
		return false;
	case ecan__2__S:
		return false;
	case ecan__3__V:
		return false;
	case ecan__4__S:
		return false;
	case ecan__5__M:
		return false;
	case ecan__6__S:
		return false;
	case ecan__7__S:
		return false;
	case ecan__8__S:
		return false;
	//More spellparams with retarded names... "Yes" or "Off"
	case e1__1__conc:
		return false;
	case e1__2__M:
		return false;
	case e1__2__S:
		return false;
	case e1__2__conc:
		return false;
	case e1__3__prep:
		return false;
	case e1__3__S:
		return false;
	case e1__4__V:
		return false;
	case e1__5__conc:
		return false;
	case e1__5__V:
		return false;
	case e1__5__S:
		return false;
	case e1__6__M:
		return false;
	case e1__6__S:
		return false;
	case e1__7__M:
		return false;
	case e1__8__conc:
		return false;
	case e1__9__conc:
		return false;
	case e1__9__S:
		return false;
	case e1__10__conc:
		return false;
	case e1__10__S:
		return false;
	case e1__11__V:
		return false;
	case e1__12__M:
		return false;
	case e1__12__S:
		return false;
	case e1__13__S:
		return false;
	case e2__1__S:
		return false;
	case e2__2__V:
		return false;
	case e2__3__S:
		return false;
	case e2__4__V:
		return false;
	case e2__5__M:
		return false;
	case e2__7__M:
		return false;
	case e2__7__V:
		return false;
	case e2__7__conc:
		return false;
	case e2__8__conc:
		return false;
	case e2__9__S:
		return false;
	case e2__9__conc:
		return false;
	case e2__10__S:
		return false;
	case e2__11__V:
		return false;
	case e2__11__conc:
		return false;
	case e2__12__M:
		return false;
	case e2__12__V:
		return false;
	case e2__13__S:
		return false;
	case e2__13__conc:
		return false;
	case eM__3__1:
		return false;
	case eM__4__1:
		return false;
	case eM__4__3:
		return false;
	case eM__4__4:
		return false;
	case eM__4__5:
		return false;
	case eM__4__6:
		return false;
	case eM__4__9:
		return false;
	case eM__4__13:
		return false;
	case eM__5__3:
		return false;
	case eM__5__4:
		return false;
	case eM__5__8:
		return false;
	case eM__5__7:
		return false;
	case eM__5__9:
		return false;
	case eS__3__1:
		return false;
	case eS__3__3:
		return false;
	case eS__3__2:
		return false;
	case eS__4__1:
		return false;
	case eS__4__2:
		return false;
	case eS__4__3:
		return false;
	case eS__4__6:
		return false;
	case eS__4__7:
		return false;
	case eS__4__8:
		return false;
	case eS__4__10:
		return false;
	case eS__4__11:
		return false;
	case eS__4__12:
		return false;
	case eS__5__4:
		return false;
	case eS__5__5:
		return false;
	case eS__5__6:
		return false;
	case eS__5__9:
		return false;
	case eS__5__1:
		return false;
	case eV__3__1:
		return false;
	case eV__4__1:
		return false;
	case eV__5__2:
		return false;
	case eV__5__4:
		return false;
	case eV__5__8:
		return false;
	case eV__5__7:
		return false;
	case eV__5__9:
		return false;
	case eV__5__1:
		return false;
	case eprep__3__1:
		return false;
	case econc__3__3:
		return false;
	case econc__3__1:
		return false;
	case econc__5__4:
		return false;
	case econc__5__9:
		return false;
	//Slot total string
	case eSpell_slots_5:
		return false;
	//More spell params "Yes" or "Off"
	case espell6__1:
		return false;
	case espell6__2:
		return false;
	//string
	case eSpell_slots_6:
		return false;
	//"Yes" or "Off"
	case espell7__1:
		return false;
	case espell7__2:
		return false;
	case espell8__1:
		return false;
	//Strings
	case eSpell_slots_8:
		return false;
	case eSpell_slots_9:
		return false;
	//"Yes" or "Off"
	case espell9__1:
		return false;
	//string
	case eSpell_slots_7:
		return false;
	//Spell names string
	case eSpell6__2:
		return false;
	case eSpell6__3:
		return false;
	case eSpell6__4:
		return false;
	case eSpell6__5:
		return false;
	case eSpell6__6:
		return false;
	case eSpell6__7:
		return false;
	case eSpell6__8:
		return false;
	case eSpell6__9:
		return false;
	case eSpell6__10:
		return false;
	case eSpell6__1:
		return false;
	case eSpell7__2:
		return false;
	case eSpell7__3:
		return false;
	case eSpell7__4:
		return false;
	case eSpell7__5:
		return false;
	case eSpell7__6:
		return false;
	case eSpell7__7:
		return false;
	case eSpell7__8:
		return false;
	case eSpell7__9:
		return false;
	case eSpell7__1:
		return false;
	case eSpell7__10:
		return false;
	case eSpell9__1:
		return false;
	case eSpell9__2:
		return false;
	case eSpell9__3:
		return false;
	case eSpell9__4:
		return false;
	case eSpell9__5:
		return false;
	case eSpell9__6:
		return false;
	case eSpell9__7:
		return false;
	case eSpell9__8:
		return false;
	case eSpell9__9:
		return false;
	case eSpell9__10:
		return false;
	case eSpell8__1:
		return false;
	case eSpell8__2:
		return false;
	case eSpell8__3:
		return false;
	case eSpell8__4:
		return false;
	case eSpell8__5:
		return false;
	case eSpell8__6:
		return false;
	case eSpell8__7:
		return false;
	case eSpell8__8:
		return false;
	case eSpell8__9:
		return false;
	case eSpell8__10:
		return false;
	//Magic Itemnames string
	case emagic__item__text__0:
		return false;
	case emagic__item__text__1:
		return false;
	case emagic__item__text__2:
		return false;
	case emagic__item__text__3:
		return false;
	case emagic__item__text__4:
		return false;
	case emagic__item__text__5:
		return false;
	case emagic__item__text__6:
		return false;
	case emagic__item__text__7:
		return false;
	case emagic__item__text__8:
		return false;
	case emagic__item__text__9:
		return false;
	case emagic__item__text__10:
		return false;
	case emagic__item__text__11:
		return false;
	case emagic__item__text__12:
		return false;
	case emagic__item__text__13:
		return false;
	case emagic__item__text__15__0:
		return false;
	case emagic__item__text__16:
		return false;
	case emagic__item__text__17:
		return false;
	case emagic__item__text__18:
		return false;
	case emagic__item__text__19:
		return false;
	case emagic__item__text__20:
		return false;
	case emagic__item__text__21:
		return false;
	case emagic__item__text__22:
		return false;
	case emagic__item__text__23:
		return false;
	case emagic__item__text__24:
		return false;
	case emagic__item__text__25:
		return false;
	case emagic__item__text__26:
		return false;
	case emagic__item__text__27:
		return false;
	case emagic__item__text__28:
		return false;
	//speeds string
	case erun:
		return false;
	case eclimb:
		return false;
	case eswim:
		return false;
	case efly:
		return false;
	//Weird, string
	case ecomp__item__0__0:
		return false;
	//Companion speeds string
	case ecomp__run:
		return false;
	case ecomp__climb:
		return false;
	case ecomp__swim:
		return false;
	case ecomp__fly:
		return false;
	case ecomp__darkvision:
		return false;
	//Apparently companions know spells, string
	case ecomp__points__name:
		return false;
	case ecomp__points__total:
		return false;
	case ecomp__points__used:
		return false;
	case espell__slot__1__total:
		return false;
	case espell__slot__1__used:
		return false;
	case espell__slot__2__total:
		return false;
	case espell__slot__2__used:
		return false;
	case espell__slot__3__total:
		return false;
	case espell__slot__3__used:
		return false;
	case espell__slot__4__total:
		return false;
	case espell__slot__4__used:
		return false;
	case espell__slot__8__total:
		return false;
	case espell__slot__8__used:
		return false;
	case espell__slot__9__total:
		return false;
	case espell__slot__5__used:
		return false;
	case espell__slot__5__total:
		return false;
	case espell__slot__6__used:
		return false;
	case espell__slot__6__total:
		return false;
	case espell__slot__7__used:
		return false;
	case espell__slot__7__total:
		return false;
	//Money string
	case epp:
		return false;
	case egp:
		return false;
	case eep:
		return false;
	case esp:
		return false;
	case ecp:
		return false;
	//carrying cap string
	case ecarrying__capacity:
		return false;
	//Weapon Overview fields most string, when ending with single letter either "Yes" or "Off"
	case eweap__1__name:
		return false;
	case eweap__1__atk:
		return false;
	case eweap__1__dmg:
		return false;
	case eweap__1__s:
		return false;
	case eweap__2__name:
		return false;
	case eweap__2__atk:
		return false;
	case eweap__2__dmg:
		return false;
	case eweap__2__p:
		return false;
	case eweap__3__name:
		return false;
	case eweap__3__atk:
		return false;
	case eweap__3__dmg:
		return false;
	case eweap__3__b:
		return false;
	case eweap__4__atk:
		return false;
	case eweap__4__s:
		return false;
	case eweap__4__dmg:
		return false;
	case eweap__4__name:
		return false;
	case eweap__5__name:
		return false;
	case eweap__5__b:
		return false;
	case eweap__5__p:
		return false;
	case eweap__5_atk:
		return false;
	case eweap__5_dmg:
		return false;
	//ammo overview fields string
	case eammo1_name:
		return false;
	case eammo__1__total:
		return false;
	case eammo__1__used:
		return false;
	case eammo__2__total:
		return false;
	case eammo__2__used:
		return false;
	case eammo__3__total:
		return false;
	case eammo__3__used:
		return false;
	case eammo__4__total:
		return false;
	case eammo__4__used:
		return false;
	//armor worn string
	case ehead:
		return false;
	case eneck:
		return false;
	case earmor:
		return false;
	case egloves:
		return false;
	case eback:
		return false;
	case ering__1:
		return false;
	case ering__2:
		return false;
	case ebelt:
		return false;
	//inventory string
	case ebackpack__1__0__0:
		return false;
	case ebackpack__1__0__1:
		return false;
	case ebackpack__1__1__0:
		return false;
	case ebackpack__1__1__1:
		return false;
	case ebackpack__1__2__0:
		return false;
	case ebackpack__1__2__1:
		return false;
	case ebackpack__1__3__0:
		return false;
	case ebackpack__1__3__1:
		return false;
	case ebackpack__1__4__0:
		return false;
	case ebackpack__1__4__1:
		return false;
	case ebackpack__1__5__0:
		return false;
	case ebackpack__1__5__1:
		return false;
	case ebackpack__1__6__0:
		return false;
	case ebackpack__1__6__1:
		return false;
	//
	case ebackpack__wt__1__0__0:
		return false;
	case ebackpack__wt__1__0__1:
		return false;
	case ebackpack__wt__1__1__0:
		return false;
	case ebackpack__wt__1__1__1:
		return false;
	case ebackpack__wt__1__2__0:
		return false;
	case ebackpack__wt__1__2__1:
		return false;
	case ebackpack__wt__1__3__0:
		return false;
	case ebackpack__wt__1__3__1:
		return false;
	case ebackpack__wt__1__4__0:
		return false;
	case ebackpack__wt__1__4__1:
		return false;
	case ebackpack__wt__1__5__0:
		return false;
	case ebackpack__wt__1__5__1:
		return false;
	case ebackpack__wt__1__6__0:
		return false;
	case ebackpack__wt__1__6__1:
		return false;
	case ecantrip__detail__1:
		return false;
	case ecantrip__detail__2:
		return false;
	case ecantrip__detail__3:
		return false;
	case ecantrip__detail__4:
		return false;
	case ecantrip__detail__5:
		return false;
	case ecantrip__detail__6:
		return false;
	case ecantrip__1:
		return false;
	case ecantrip__2:
		return false;
	case ecantrip__3:
		return false;
	case ecantrip__4:
		return false;
	case ecantrip__5:
		return false;
	case ecantrip__6:
		return false;
	case eTspell__slot__9__used:
		return false;
	case eweapon_profs:
		return false;
	case earmor_profs:
		return false;
	case etools:
		return false;
	case eadvantages:
		return false;
	case efeet:
		return false;
	case eage:
		return false;
	case eheight:
		return false;
	case eweight:
		return false;
	case eeyes:
		return false;
	case eskin:
		return false;
	case ehair:
		return false;
	case eDeity:
		return false;
	case eAllies_and_orgs:
		return false;
	case epersonality:
		return false;
	case eideals:
		return false;
	case ebonds:
		return false;
	case eflaws:
		return false;
	case eextra__traits:
		return false;
	case eText115:
		return false;
	case eText114:
		return false;
	case eText116:
		return false;
	case eText196:
		return false;
	case eText197:
		return false;
	case eText195:
		return false;
	case ebackstory:
		return false;
	case eText136:
		return false;
	case eText119:
		return false;
	case eText117:
		return false;
	case eText168:
		return false;
	case eText194:
		return false;
	case eText181:
		return false;
	case eText144:
		return false;
	case eText122:
		return false;
	case eText118:
		return false;
	case eText167:
		return false;
	case eText193:
		return false;
	case eText180:
		return false;
	case eText145:
		return false;
	case eText125:
		return false;
	case eText120:
		return false;
	case eText166:
		return false;
	case eText192:
		return false;
	case eText179:
		return false;
	case eText146:
		return false;
	case eText128:
		return false;
	case eText121:
		return false;
	case eText165:
		return false;
	case eText191:
		return false;
	case eText178:
		return false;
	case eText147:
		return false;
	case eText131:
		return false;
	case eText123:
		return false;
	case eText164:
		return false;
	case eText190:
		return false;
	case eText177:
		return false;
	case eText148:
		return false;
	case eText134:
		return false;
	case eText124:
		return false;
	case eText163:
		return false;
	case eText189:
		return false;
	case eText176:
		return false;
	case eText149:
		return false;
	case eText137:
		return false;
	case eText126:
		return false;
	case eText162:
		return false;
	case eText188:
		return false;
	case eText175:
		return false;
	case eText150:
		return false;
	case eText143:
		return false;
	case eText127:
		return false;
	case eText161:
		return false;
	case eText182:
		return false;
	case eText174:
		return false;
	case eText151:
		return false;
	case eText142:
		return false;
	case eText129:
		return false;
	case eText160:
		return false;
	case eText183:
		return false;
	case eText173:
		return false;
	case eText152:
		return false;
	case eText141:
		return false;
	case eText130:
		return false;
	case eText159:
		return false;
	case eText184:
		return false;
	case eText172:
		return false;
	case eText153:
		return false;
	case eText140:
		return false;
	case eText132:
		return false;
	case eText158:
		return false;
	case eText185:
		return false;
	case eText171:
		return false;
	case eText154:
		return false;
	case eText139:
		return false;
	case eText133:
		return false;
	case eText157:
		return false;
	case eText186:
		return false;
	case eText170:
		return false;
	case ecomp_AC:
		return false;
	case ecomp_init:
		return false;
	case ecomp_temp_hp:
		return false;
	case ecomp_hp_tot:
		return false;
	case ecomp_buffs:
		return false;
	case ecomp_creature:
		return false;
	case ecomp_bonded_to:
		return false;
	case ecomp_size:
		return false;
	case ecomp_name:
		return false;
	case ecomp_align:
		return false;
	case ecomp_weight:
		return false;
	case ecomp_type:
		return false;
	case ecomp_passive_perception:
		return false;
	case ecomp_weapon_1:
		return false;
	case ecomp_dmg_1:
		return false;
	case ecomp_weapon_2:
		return false;
	case ecomp_dmg_2:
		return false;
	case ecomp_weapon_3:
		return false;
	case ecomp_dmg_3:
		return false;
	case ecomp_weapn_4:
		return false;
	case ecomp_dmg_4:
		return false;
	case ecomp_Dex_bonus:
		return false;
	case ecomp_con_bonus:
		return false;
	case ecomp_wis_bonus:
		return false;
	case ecomp_cha_bonus:
		return false;
	case ecomp_int_bonus:
		return false;
	case ecomp_STR_Bonus:
		return false;
	case eDex_bonus:
		return false;
	case ecomp_current_hp:
		return false;
	case ecomp_features:
		return false;
	default:
		return false;
	}

	return false;
}

bool readArr( Character& character, PDFArray& arr, PDFParser& parser, std::string prefix ){
	bool ret = true;
	for( size_t i = 0; i < arr.GetLength(); i++ ){
		PDFObject* obj = arr.QueryObject( i );		//PDFIndirectObjectReference | PDFDictionary
		if( obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference ){
			obj = parser.ParseNewObject( ((PDFIndirectObjectReference*)obj)->mObjectID );
		}
		if( !readObject( character, *(PDFDictionary*)obj, parser, prefix ) )
			ret = false;
	}
	return ret;
}


