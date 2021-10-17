#include <gen/Variable.h>
#include <gen/ResultData.h>
#include <gen/VariableHandler.h>
#include <gen/ResultDataHandler.h>
#include <QLabel>
#include <misc/gen/ScriptGlobalEntry.h>
#include <misc/qt/Globals.h>
#include "InformationMonitor.h"
#include "ui_InformationMonitor.h"

namespace sf
{

// Anonymous namespace
namespace
{

ScriptGlobalStaticEntry* gseMonitorVariable{nullptr};

ScriptGlobalStaticEntry* gseMonitorResultData{nullptr};

__attribute__((constructor)) void Initialize()
{
	gseMonitorVariable = new ScriptGlobalStaticEntry([](const Value::vector_type& arguments) -> Value
	{
		if (auto parent = getGlobalParent())
		{
			auto im = new InformationMonitor(parent);
			im->setId(Gii::Variable, arguments[0].getInteger());
			im->show();
			return Value(true);
		}
		return Value(false);
	}, "MonitorVariable", "Opens monitor on a variable of the passed id.", 1);
	gseMonitorResultData = new ScriptGlobalStaticEntry([](const Value::vector_type& arguments) -> Value
	{
		if (auto parent = getGlobalParent())
		{
			auto im = new InformationMonitor(parent);
			im->setId(Gii::ResultData, arguments[0].getInteger());
			im->show();
			return Value(true);
		}
		return Value(false);
	}, "MonitorResultData", "Opens monitor on a result-data of the passed id.", 1);
}

__attribute__((destructor)) void Deinitialize()
{
	delete_null(gseMonitorVariable);
	delete_null(gseMonitorResultData);
}

}

struct InformationMonitor::Private :VariableHandler, ResultDataHandler
{
	Ui::InformationMonitor* ui;

	InformationMonitor* _w;

	Variable _variable;
	ResultData _resultData;

	// ID that was used to fill static data.
	id_type _currentId{std::numeric_limits<id_type>::max()};

	Gii::IdType _id{0};

	QTreeWidgetItem* _itemCurrent{nullptr};
	QTreeWidgetItem* _itemFlags{nullptr};

	explicit Private(InformationMonitor* widget)
		:_w(widget)
		 , ui(new Ui::InformationMonitor)
	{
	}

	~Private() override
	{
		delete ui;
	}

	void initialize()
	{
		ui->setupUi(_w);
		//
		_variable.setHandler(this);
		_resultData.setHandler(this);
		//
		_variable.setConvert(true);
		//
		ui->twProperties->setColumnCount(2);
		ui->twProperties->setHeaderLabels({tr("Property"), tr("Value")});
		ui->twProperties->setAlternatingRowColors(true);
	}

	void setup(Gii::IdType id)
	{
		_variable.setup(ui->iieId->getTypeId() == Gii::Variable ? id : 0, true);
		_resultData.setup(ui->iieId->getTypeId() == Gii::ResultData ? id : 0, true);
	}

	void update() const
	{
		if (ui->iieId->getTypeId() == Gii::Variable)
		{
			if (_itemCurrent)
			{
				auto s = _variable.getCur().getString();
				if (_variable.getStateCount())
				{
					auto state = _variable.getState(_variable.getCur(false));
					if (state == Variable::npos)
					{
						_itemCurrent->setText(1, QString::fromStdString(s));
					}
					else
					{
						_itemCurrent->setText(1, QString::fromStdString(_variable.getStateName(state) + " (" + s + ")"));
					}
				}
				else
				{
					_itemCurrent->setText(1, QString::fromStdString(s));
				}
			}
			if (_itemFlags)
			{
				_itemFlags->setText(1, QString::fromStdString(_variable.getCurFlagsString() + " (" + _variable.getFlagsString() + ")"));
			}
		}
		else if (ui->iieId->getTypeId() == Gii::ResultData)
		{
			if (_itemFlags)
			{
				_itemFlags->setText(1, QString::fromStdString(_resultData.getCurFlagsString() + " (" + _resultData.getFlagsString() + ")"));
			}
			if (_itemCurrent)
			{
				auto range = _resultData.getAccessRange();
				_itemCurrent->setText(1, QString("%1 to %2").arg(range.getStart()).arg(range.getStop()));
			}
			// Scoped part.
			{
				auto owner = const_cast<ResultData&>(_resultData).getOwner();
				std::stringstream ss;
				ss << "Validated:" << owner.getValidatedList() << std::endl;
				ss << "Committed:" << owner.getCommitList() << std::endl;
				Range::Vector ranges;
				owner.getRequests(ranges);
				ss << "Requests:" << ranges << std::endl;
				ui->pteData->setPlainText(QString::fromStdString(ss.str()));
			}
		}
	}

	void fillView(id_type id)
	{
		// Reduce over head by checking the last id used to fill the static data.
		if (_currentId == id)
		{
			return;
		}
		_currentId = id;
		ui->twProperties->clear();
		if (ui->iieId->getTypeId() == Gii::Variable)
		{
			ui->gbData->setVisible(false);
			for (auto field: {vfId, vfName, VariableTypes::EField(-1), vfUnit, vfFlags, vfDescription, vfType, vfRound, vfDefault, vfMinimum, vfMaximum})
			{
				auto twi = new QTreeWidgetItem();
				if (field == -1)
				{
					_itemCurrent = twi;
					twi->setText(0, "Current");
				}
				else
				{
					twi->setText(0, QString::fromStdString(Variable::getFieldName(field)));
					auto val = QString("Value %1").arg(field);
					switch (field)
					{
						default:
							break;

						case vfId:
							val = QString("0x%1").arg(_variable.getId(), 0, 16);
							break;

						case vfName:
							val = QString::fromStdString(_variable.getName());
							break;

						case vfUnit:
							val = QString::fromStdString(_variable.getUnit());
							break;

						case vfType:
							val = QString::fromStdString(_variable.getType(_variable.getType()));
							break;

						case vfFlags:
							_itemFlags = twi;
							val = QString::fromStdString(_variable.getCurFlagsString() + " (" + _variable.getFlagsString() + ")");
							break;

						case vfDescription:
							val = QString::fromStdString(_variable.getDescription());
							break;

						case vfDefault:
							val = QString::fromStdString(_variable.getDef().getString());
							break;

						case vfRound:
							val = QString::fromStdString(_variable.getRnd().getString() + " (" + itostr(_variable.getSigDigits()) + ')');
							break;

						case vfMinimum:
							val = QString::fromStdString(_variable.getMin().getString());
							break;

						case vfMaximum:
							val = QString::fromStdString(_variable.getMax().getString());
							break;
					}
					twi->setText(1, val);
				}
				ui->twProperties->addTopLevelItem(twi);
			}
			// For each state ad a separate entry.
			if (_variable.getStateCount())
			{
				for (int i = 0; i < _variable.getStateCount(); ++i)
				{
					auto twi = new QTreeWidgetItem();
					twi->setText(0, QString::fromStdString(Variable::getFieldName(vfFirstState + i)));
					twi->setText(1, QString::fromStdString(_variable.getStateValue(i).getString() + "=" + _variable.getStateName(i)));
					ui->twProperties->addTopLevelItem(twi);
				}
			}
		}
		else if (ui->iieId->getTypeId() == Gii::ResultData)
		{
			ui->gbData->setVisible(true);
			for (auto field: {rfId, rfName, rfType, rfFlags, rfDescription, rfSigBits, rfOffset, rfBlockSize, rfSegmentSize, ResultDataTypes::EField(-1)})
			{
				auto twi = new QTreeWidgetItem();
				if (field == -1)
				{
					twi->setText(0, "Accessible");
					_itemCurrent = twi;
					ui->twProperties->addTopLevelItem(twi);
				}
				else
				{
					twi->setText(0, QString::fromStdString(ResultData::getFieldName(field)));
					QString val;
					switch (field)
					{
						default:
							val = QString("Value %1").arg(field);
							break;

						case rfId:
							val = QString("0x%1").arg(_resultData.getId(), 0, 16);
							break;

						case rfName:
							val = QString::fromStdString(_resultData.getName());
							break;

						case rfFlags:
							_itemFlags = twi;
							break;

						case rfDescription:
							val = QString::fromStdString(_resultData.getDescription());
							break;

						case rfType:
							val = QString::fromStdString(ResultData::getType(_resultData.getType()));
							break;

						case rfSigBits:
							val = QString::number(_resultData.getSignificantBits());
							break;

						case rfOffset:
							val = QString::number(_resultData.getValueOffset());
							break;

						case rfBlockSize:
							val = QString::number(_resultData.getBlockSize());
							break;

						case rfSegmentSize:
							val = QString::number(_resultData.getSegmentSize());
							break;
					}
					twi->setText(1, val);
					ui->twProperties->addTopLevelItem(twi);
				}
			}
		}
		// Update the dynamic rows.
		update();
	}

	void variableEventHandler(VariableTypes::EEvent event, const Variable& call_var, Variable& link_var, bool sameInst) override
	{
		switch (event)
		{
			default:
				break;

			case veConverted:
			case veIdChanged:
			case veDesiredId:
				fillView(link_var.getId());
				break;

			case veValueChange:
			case veFlagsChange:
				update();
				break;
		}
	}

	void resultDataEventHandler(ResultDataTypes::EEvent event, const ResultData& caller, ResultData& link, const Range& range, bool b) override
	{
		switch (event)
		{
			default:
				break;

			case reDesiredId:
			case reIdChanged:
				fillView(link.getId());
				break;

			case reFlagsChange:
			case reClear:
			case reCommitted:
			case reReserve:
				update();
				break;
		}
	}

};

InformationMonitor::InformationMonitor(QWidget* parent)
	:QDialog(parent ? parent : getGlobalParent())
	 , _p(new Private(this))
{
	_p->initialize();
	// Delete this instance when closed since it is a free floating window.
	setAttribute(Qt::WA_DeleteOnClose);
	//
	connect(_p->ui->buttonBox, &QDialogButtonBox::clicked, this, &QDialog::close);
	connect(_p->ui->iieId, &QLineEdit::textChanged, [&](const QString &)
	{
		_p->setup(_p->ui->iieId->getId());
	});

}

InformationMonitor::~InformationMonitor()
{
	delete _p;
}

void InformationMonitor::setId(Gii::TypeId typeId, Gii::IdType id)
{
	// Set the correct type of id for the edit box to select.
	_p->ui->iieId->setTypeId(typeId);
	// Set the edit.
	_p->ui->iieId->setText(QString("0x%1").arg(id, 0, 16));
}

bool InformationMonitor::selectId(Gii::TypeId typeId, QWidget* parent)
{
	_p->ui->iieId->setTypeId(typeId);
	return _p->ui->iieId->selectDialog(parent ? parent : getGlobalParent());
}

}