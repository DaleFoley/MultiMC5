/* Copyright 2013 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "lwjglselectdialog.h"
#include "ui_lwjglselectdialog.h"

#include "lwjglversionlist.h"

LWJGLSelectDialog::LWJGLSelectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LWJGLSelectDialog)
{
	ui->setupUi(this);
	ui->labelStatus->setVisible(false);
	ui->lwjglListView->setModel(&LWJGLVersionList::get());
	
	connect(&LWJGLVersionList::get(), SIGNAL(loadingStateUpdated(bool)), SLOT(loadingStateUpdated(bool)));
	connect(&LWJGLVersionList::get(), SIGNAL(loadListFailed(QString)), SLOT(loadingFailed(QString)));
	loadingStateUpdated(LWJGLVersionList::get().isLoading());
}

LWJGLSelectDialog::~LWJGLSelectDialog()
{
	delete ui;
}

QString LWJGLSelectDialog::selectedVersion() const
{
	return LWJGLVersionList::get().data(
				ui->lwjglListView->selectionModel()->currentIndex(),
				Qt::DisplayRole).toString();
}

void LWJGLSelectDialog::on_refreshButton_clicked()
{
	if (!LWJGLVersionList::get().isLoading())
		LWJGLVersionList::get().loadList();
}

void LWJGLSelectDialog::loadingStateUpdated(bool loading)
{
	setEnabled(!loading);
	if (loading)
	{
		ui->labelStatus->setText("Loading LWJGL version list...");
		ui->labelStatus->setStyleSheet("QLabel { color: black; }");
	}
	ui->labelStatus->setVisible(loading);
}

void LWJGLSelectDialog::loadingFailed(QString error)
{
	ui->labelStatus->setText(error);
	ui->labelStatus->setStyleSheet("QLabel { color: red; }");
	ui->labelStatus->setVisible(true);
}