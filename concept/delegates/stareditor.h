#pragma once
#include <QWidget>

#include "starrating.h"

class StarEditor :public QWidget
{
	Q_OBJECT
	public:
		StarEditor(QWidget* parent = nullptr);

		[[nodiscard]] QSize sizeHint() const override;

		void setStarRating(const StarRating& starRating)
		{
			myStarRating = starRating;
		}

		StarRating starRating() {return myStarRating;}

	Q_SIGNALS:
		void editingFinished();

	protected:
		void paintEvent(QPaintEvent* event) override;

		void mouseMoveEvent(QMouseEvent* event) override;

		void mouseReleaseEvent(QMouseEvent* event) override;

	private:
		[[nodiscard]] int starAtPosition(int x) const;

		StarRating myStarRating;
};
