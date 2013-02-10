#pragma once

namespace Layouter
{
	class CLayouter : public Util::Class::CRefCountImpl
	{
		XClass(CLayouter);
	public:
		virtual Property::ELayoutType MyType() = 0;
		virtual XResult Layout(ElementRef element) = 0;
	};

	typedef XSmartPtr<CLayouter>	LayouterRef;

	MyNameIs(CLayouter)
		I_Provide("�Ű����Ļ��ඨ��")
	End_Description;
}