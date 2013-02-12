#pragma once
#include "XBase.hpp"
#include "XTree.hpp"
#include "XPropertyValue.hpp"
#include "../Others/CBuffer.h"

#include <map>

namespace Property
{ \

//////////////////////////////////////////////////////////////////////////
enum class ELayoutType
{
	Offset,
	Block
};

namespace LayoutTypeString
{
	LPCTSTR Layout_OffsetString = _T("offset");
	LPCTSTR Layout_BlockString = _T("block");
}

enum class ELayoutDirection
{
	Horizon,
	Vertical,
};

namespace LayoutDirectionSting
{
	LPCTSTR LayoutDirection_HorizonString = _T("horizon");
	LPCTSTR LayoutDirection_VerticalString = _T("vertical");
}

enum class EAlignType
{
	Left,
	Top,
	Right,
	Bottom,
	Center,
	HCenter,
	VCenter,
	//Element,
};

namespace AlignTypeString
{
	LPCTSTR Align_LeftString = _T("left");
	LPCTSTR Align_TopString = _T("top");
	LPCTSTR Align_RightString = _T("right");
	LPCTSTR Align_BottomString = _T("bottom");
	LPCTSTR Align_CenterString = _T("center");
	LPCTSTR Align_HCenterString = _T("hcenter");
	LPCTSTR Align_VCenterString = _T("vcenter");
}

//////////////////////////////////////////////////////////////////////////

#define SupportType(_prop,_type) \
	XResult SetProperty(CString key,const _type::ValueType& value) \
{ \
	CBuffer &buffer = _prop[key]; \
	_type *propValue = (_type*)buffer.GetBuffer(); \
	if (propValue == nullptr) \
{ \
	buffer.Alloc(sizeof(_type)); \
	propValue = new(buffer.GetBuffer()) _type(value); \
	} \
	*propValue = value; \
	return XResult_OK; \
	} \
	XResult GetProperty(CString key,_type::ValueType& value) const \
{ \
	auto i = _prop.find(key); \
	if (i == _prop.end()) \
{ \
	return XResult_NotFound; \
	} \
	const CBuffer &buffer = i->second; \
	const _type *propValue = nullptr; \
	propValue = (const _type*)buffer.GetBuffer(); \
	value = propValue->m_value; \
	return XResult_OK; \
	}

class CXProperty
{
	XClass(VOID);
public:
	SupportType(m_propertyMap,CPropertyValue<CString>);
	SupportType(m_propertyMap,CPropertyValue<DWORD>);
	SupportType(m_propertyMap,CPropertyValue<INT>);
	SupportType(m_propertyMap,CPropertyValue<NodeRef>);
	SupportType(m_propertyMap,CPropertyValue<CRect>);
	SupportType(m_propertyMap,CPropertyValue<CPoint>);
	SupportType(m_propertyMap,CPropertyValue<CSize>);
	SupportType(m_propertyMap,CPropertyValue<HWND>);
	SupportType(m_propertyMap,CPropertyValue<ELayoutType>);
	SupportType(m_propertyMap,CPropertyValue<ELayoutDirection>);
	SupportType(m_propertyMap,CPropertyValue<EAlignType>);

	BOOL IsChanged(CString key);
	VOID ChangeRestore(CString key/*empty means all*/);

	CXProperty& operator=(const CXProperty& rhs);
	XResult	Switch(CXProperty& rhs);
protected:
	std::map<CString,CBuffer>	m_propertyMap;
};

MyNameIs(CXProperty)
	I_Provide("属性描述表及属性的保持工具")
	End_Description;


//////////////////////////////////////////////////////////////////////////

BOOL CXProperty::IsChanged( CString key )
{
	auto i = m_propertyMap.find(key);
	if (i != m_propertyMap.end())
	{
		CBuffer& buffer = i->second;
		CChangable* changable = static_cast<CChangable*>((VOID*)buffer.GetBuffer());
		return changable->IsChanged();
	}
	return FALSE;
}

VOID CXProperty::ChangeRestore( CString key )
{
	if (key.GetLength() != 0)
	{
		auto i = m_propertyMap.find(key);
		if (i != m_propertyMap.end())
		{
			CBuffer& buffer = i->second;
			CChangable* changable = static_cast<CChangable*>((VOID*)buffer.GetBuffer());
			changable->Restore();
		}
	}
	else
	{
		for (auto i=m_propertyMap.begin(); i!=m_propertyMap.end(); ++i)
		{
			CBuffer& buffer = i->second;
			CChangable* changable = static_cast<CChangable*>((VOID*)buffer.GetBuffer());
			changable->Restore();
		}
	}
}

//////////////////////////////////////////////////////////////////////////

class CXMLConverter_CString
{
public:
	static CString ConvertToValue(CString value)
	{
		return value;
	}
};

class CXMLConverter_INT
{
public:
	static INT ConvertToValue(CString value)
	{
		INT intValue;
		intValue = _ttoi(value);
		return intValue;
	}
};

class CXMLConverter_DWORD
{
public:
	static DWORD ConvertToValue(CString value)
	{
		DWORD dwValue;
		_stscanf_s(value,_T("%u"),&dwValue);
		return dwValue;
	}
};

typedef CXMLConverter_DWORD CXMLConverter_HWND;

class CXMLConverter_COLORREF
{
public:
	static COLORREF ConvertToValue(CString value)
	{
		COLORREF dwValue;
		DWORD r,g,b,a;
		_stscanf_s(value,_T("%u,%u,%u,%u"),&r,&g,&b,&a);
		dwValue = RGBA(r,g,b,a);
		return dwValue;
	}
};

class CXMLConverter_BOOL
{
public:
	static COLORREF ConvertToValue(CString value)
	{
		BOOL boolValue = FALSE;
		if (value == _T("TRUE"))
		{
			boolValue = TRUE;
		}
		else
		{
			boolValue = _ttoi(value)==0? FALSE:TRUE;
		}
		return boolValue;
	}
};

class CXMLConverter_CRect
{
public:
	static CRect ConvertToValue(CString value)
	{
		CRect rect;
		_stscanf_s(value,_T("%d,%d,%d,%d"),&rect.left,&rect.top,&rect.right,&rect.bottom);
		return rect;
	}
};

class CXMLConverter_CPoint
{
public:
	static CPoint ConvertToValue(CString value)
	{
		CPoint point;
		_stscanf_s(value,_T("%d,%d"),&point.x,&point.y);
		return point;
	}
};

class CXMLConverter_CSize
{
public:
	static CSize ConvertToValue(CString value)
	{
		CSize size;
		_stscanf_s(value,_T("%d,%d"),&size.cx,&size.cy);
		return size;
	}
};

class CXMLConverter_ELayoutType
{
public:
	static ELayoutType ConvertToValue(CString value)
	{
		ELayoutType type = ELayoutType::Block;
		if (value == LayoutTypeString::Layout_OffsetString)
		{
			type = ELayoutType::Offset;
		}
		else if (value == LayoutTypeString::Layout_BlockString)
		{
			type = ELayoutType::Block;
		}
		else
		{
			ATLASSERT(FALSE && "invalid layout type");
		}
		return type;
	}
};

class CXMLConverter_EAlignType
{
public:
	static EAlignType ConvertToValue(CString value)
	{
		EAlignType type = EAlignType::Left;
		if (value == AlignTypeString::Align_LeftString)
		{
			type = EAlignType::Left;
		}
		else if (value == AlignTypeString::Align_TopString)
		{
			type = EAlignType::Top;
		}
		else if (value == AlignTypeString::Align_RightString)
		{
			type = EAlignType::Right;
		}
		else if (value == AlignTypeString::Align_BottomString)
		{
			type = EAlignType::Bottom;
		}
		else if (value == AlignTypeString::Align_CenterString)
		{
			type = EAlignType::Center;
		}
		else if (value == AlignTypeString::Align_HCenterString)
		{
			type = EAlignType::HCenter;
		}
		else if (value == AlignTypeString::Align_VCenterString)
		{
			type = EAlignType::VCenter;
		}
		else
		{
			ATLASSERT(FALSE && "invalid align type");
		}
	}
};

class CXMLConverter_ELayoutDirection
{
public:
	static ELayoutDirection ConvertToValue(CString value)
	{
		ELayoutDirection directionValue = ELayoutDirection::Horizon;
		if (value == LayoutDirectionSting::LayoutDirection_HorizonString)
		{
			directionValue = ELayoutDirection::Horizon;
		}
		else if (value == LayoutDirectionSting::LayoutDirection_VerticalString)
		{
			directionValue = ELayoutDirection::Vertical;
		}
		return directionValue;
	}
};

// contains each properties definition
#define DefineProperty(_name,_type,_defaultValue)	\
	static LPCTSTR _name = _T(#_name); \
	static _type _name##DefaultValue = _defaultValue; \
	typedef _type _name##Type; \
	typedef CXMLConverter_##_type _name##XMLConverter; \

DefineProperty(ID,				CString,			_T(""));
DefineProperty(Rect,			CRect,				CRect());
DefineProperty(Position,		CPoint,				CPoint());
DefineProperty(Size,			CSize,				CSize());
DefineProperty(Text,			CString,			_T(""));
DefineProperty(Title,			CString,			_T(""));
DefineProperty(Color,			COLORREF,			0);
DefineProperty(TextColor,		COLORREF,			0);
DefineProperty(Style,			DWORD,				WS_OVERLAPPEDWINDOW|WS_VISIBLE);
DefineProperty(ExStyle,			DWORD,				0);
DefineProperty(HWnd,			HWND,				0);
DefineProperty(ShowState,		BOOL,				FALSE);
DefineProperty(LayoutType,		ELayoutType,		ELayoutType::Block);
DefineProperty(LayoutInvalid,	BOOL,				FALSE);
DefineProperty(LayoutDirection,	ELayoutDirection,	ELayoutDirection::Horizon)
DefineProperty(Align,			EAlignType,			EAlignType::Left);
DefineProperty(AutoWidth,		BOOL,				FALSE);
DefineProperty(AutoHeight,		BOOL,				FALSE);
DefineProperty(ExpandWidth,		BOOL,				FALSE);
DefineProperty(ExpandHeight,	BOOL,				FALSE);
DefineProperty(BorderFix,		BOOL,				FALSE);

};