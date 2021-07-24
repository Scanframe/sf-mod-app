#pragma once

#include <QObject>
#include <QKeySequence>
#include "global.h"

class QxtGlobalShortcutPrivate;

class _GSC_CLASS QxtGlobalShortcut :public QObject
{
	Q_OBJECT
		Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
		Q_PROPERTY(bool valid READ isValid)
		Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setShortcut)

	public:
		explicit QxtGlobalShortcut(QObject* parent = nullptr);

		explicit QxtGlobalShortcut(const QKeySequence& sequence, QObject* parent = nullptr);

		~QxtGlobalShortcut() override;

		[[nodiscard]] QKeySequence shortcut() const;

		bool setShortcut(const QKeySequence& sequence);

		[[nodiscard]] bool isEnabled() const;

		[[nodiscard]] bool isValid() const;

	public Q_SLOTS:

		void setEnabled(bool enabled = true);

		void setDisabled(bool disabled = true);

	Q_SIGNALS:

		void activated(QxtGlobalShortcut* self);

	private:
		friend class QxtGlobalShortcutPrivate;

		QxtGlobalShortcutPrivate* _p;
};
