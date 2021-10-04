#pragma once

#if IS_QT
	#include <QByteArray>
#endif
#include <misc/gen/TClosure.h>
#include "Variable.h"
#include "../global.h"

namespace sf
{

class _GII_CLASS VariableIdList :private InformationTypes
{
	public:

		explicit VariableIdList(const Variable::PtrVector& list)
			:_ptrList(&list)
		{
		}

		explicit VariableIdList(const Variable::Vector& list)
			:_list(&list)
		{
		}

		void setIds(const std::string& ids);

		[[nodiscard]] std::string getIds() const;

#if IS_QT

		inline operator QByteArray() const
		{
			return QByteArray::fromStdString(getIds());
		}

		inline VariableIdList& operator=(const QByteArray& ba)
		{
			setIds(ba.toStdString());
			return *this;
		}

		inline bool operator!=(const QByteArray& ba) const
		{
			return getIds() != ba.toStdString();
		}

#endif

		TClosure<void, void*> onChange;

	private:

		const Variable::PtrVector* _ptrList{nullptr};
		const Variable::Vector* _list{nullptr};
};

}