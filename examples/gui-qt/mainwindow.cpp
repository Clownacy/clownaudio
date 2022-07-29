// Copyright (c) 2021 Clownacy
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <QString>
#include <QFileDialog>
#include <QMessageBox>

#include <clownaudio/clownaudio.h>

static QString FormattedQString(const char *format_string, ...)
{
	char *string_buffer = nullptr;
	for (std::size_t string_buffer_size = 0; ; )
	{
		std::va_list args;
		va_start(args, format_string);

		std::size_t size_needed = std::vsnprintf(string_buffer, string_buffer_size, format_string, args) + 1;

		va_end(args);

		if (size_needed <= string_buffer_size)
			break;

		delete[] string_buffer;
		string_buffer = new char[size_needed];
		string_buffer_size = size_needed;
	}

	QString final_string = string_buffer;

	delete[] string_buffer;

	return final_string;
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Disable parts of the interface by default
	ui->pushButton_UnloadSoundData->setEnabled(false);
	ui->pushButton_DestroySound->setEnabled(false);
	ui->groupBox_SoundCreation->setEnabled(false);
	ui->groupBox_PlaybackControls->setEnabled(false);

	ClownAudio_Init();
}

MainWindow::~MainWindow()
{
	// Destroy sounds
	for(int i = 0; i < ui->listWidget_Sounds->count(); ++i)
	{
		QListWidgetItem *item = ui->listWidget_Sounds->item(i);
		SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

		ClownAudio_SoundDestroy(sound_metadata->id);
		delete sound_metadata;
	}

	// Unload sound data
	for(int i = 0; i < ui->listWidget_SoundData->count(); ++i)
	{
		QListWidgetItem *item = ui->listWidget_SoundData->item(i);
		ClownAudio_SoundData *sound_data = static_cast<ClownAudio_SoundData*>(item->data(Qt::UserRole).value<void*>());

		ClownAudio_SoundDataUnload(sound_data);
	}

	ClownAudio_Deinit();

	delete ui;
}

void MainWindow::UpdateSoundVolume(SoundMetadata *sound_metadata)
{
	unsigned short master_volume_nonlinear = (sound_metadata->master_volume * sound_metadata->master_volume) >> 8;
	unsigned short left_volume_nonlinear = (sound_metadata->left_volume * sound_metadata->left_volume) >> 8;
	unsigned short right_volume_nonlinear = (sound_metadata->right_volume * sound_metadata->right_volume) >> 8;

	unsigned short final_left_volume = (left_volume_nonlinear * master_volume_nonlinear) >> 8;
	unsigned short final_right_volume = (right_volume_nonlinear * master_volume_nonlinear) >> 8;
	ClownAudio_SoundSetVolume(sound_metadata->id, final_left_volume, final_right_volume);
}

void MainWindow::on_pushButton_File1Open_clicked()
{
	QString song_intro_filename = QFileDialog::getOpenFileName(this, "Select sound intro file");

	ui->lineEdit_File1->setText(song_intro_filename);
}

void MainWindow::on_pushButton_File2Open_clicked()
{
	QString song_intro_filename = QFileDialog::getOpenFileName(this, "Select sound loop file");

	ui->lineEdit_File2->setText(song_intro_filename);
}

void MainWindow::on_pushButton_LoadSoundData_clicked()
{
	// Set up sound data configuration
	ClownAudio_SoundDataConfig config;
	ClownAudio_SoundDataConfigInit(&config);
	config.predecode = ui->checkBox_Predecode->isChecked();
	config.must_predecode = ui->checkBox_MustPredecode->isChecked();
	config.dynamic_sample_rate = ui->checkBox_DataDynamicSampleRate->isChecked();

	// Create sound data
	ClownAudio_SoundData *sound_data = ClownAudio_SoundDataLoadFromFiles(ui->lineEdit_File1->text().toStdString().c_str(), ui->lineEdit_File2->text().toStdString().c_str(), &config);

	if (sound_data == nullptr)
	{
		QMessageBox::warning(this, "Error", "Could not load sound data");
	}
	else
	{
		// Add sound data to list widget
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(FormattedQString("Sound data %p", sound_data));
		item->setData(Qt::UserRole, QVariant::fromValue<void*>(sound_data));

		ui->listWidget_SoundData->addItem(item);
		ui->listWidget_SoundData->setCurrentItem(item);
	}
}

void MainWindow::on_pushButton_UnloadSoundData_clicked()
{
	QListWidgetItem *item = ui->listWidget_SoundData->currentItem();
	ClownAudio_SoundData *sound_data = static_cast<ClownAudio_SoundData*>(item->data(Qt::UserRole).value<void*>());

	// Remove the widgets of any sounds that use this sound data,
	// as unloading this sound data will destroy these sounds
	for(int i = 0; i < ui->listWidget_Sounds->count(); ++i)
	{
		QListWidgetItem *item = ui->listWidget_Sounds->item(i);
		SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

		if (sound_metadata->data == sound_data)
		{
			delete sound_metadata;
			delete item;
			--i;
		}
	}

	ClownAudio_SoundDataUnload(sound_data);

	delete item;
}

void MainWindow::on_pushButton_CreateSound_clicked()
{
	QListWidgetItem *item = ui->listWidget_SoundData->currentItem();

	// Set up sound configuration
	ClownAudio_SoundConfig config;
	ClownAudio_SoundConfigInit(&config);
	config.loop = ui->checkBox_Loop->isChecked();
	config.do_not_destroy_when_done = ui->checkBox_DoNotDestroyWhenDone->isChecked();
	config.dynamic_sample_rate = ui->checkBox_InstanceDynamicSampleRate->isChecked();

	ClownAudio_SoundData *sound_data = static_cast<ClownAudio_SoundData*>(item->data(Qt::UserRole).value<void*>());

	ClownAudio_SoundID sound_id = ClownAudio_SoundCreate(sound_data, &config);

	if (sound_id == 0)
	{
		QMessageBox::warning(this, "Error", "Could not create sound");
	}
	else
	{
		// Initialise sound metadata
		SoundMetadata *sound_metadata = new SoundMetadata;
		sound_metadata->id = sound_id;
		sound_metadata->data = sound_data;
		sound_metadata->master_volume = 0x100;
		sound_metadata->left_volume = 0x100;
		sound_metadata->right_volume = 0x100;
		sound_metadata->speed = 0x10000;
		sound_metadata->low_pass_filter_sample_rate = 48000;
		sound_metadata->paused = true;

		// Add sound (and metadata) to list widget
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(FormattedQString("Sound %X", sound_id));
		item->setData(Qt::UserRole, QVariant::fromValue(sound_metadata));

		ui->listWidget_Sounds->addItem(item);
		ui->listWidget_Sounds->setCurrentItem(item);
	}
}

void MainWindow::on_pushButton_DestroySound_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_SoundDestroy(sound_metadata->id);

	delete sound_metadata;
	delete item;
}

void MainWindow::on_listWidget_SoundData_itemSelectionChanged()
{
	bool sound_data_selected = ui->listWidget_SoundData->currentItem() != nullptr;

	// Enable or disable parts of the interface
	ui->pushButton_UnloadSoundData->setEnabled(sound_data_selected);
	ui->groupBox_SoundCreation->setEnabled(sound_data_selected);
}

void MainWindow::on_listWidget_Sounds_itemSelectionChanged()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();

	bool sound_instance_selected = item != nullptr;

	// Enable or disable parts of the interface
	ui->pushButton_DestroySound->setEnabled(sound_instance_selected);
	ui->groupBox_PlaybackControls->setEnabled(sound_instance_selected);

	if (sound_instance_selected)
	{
		// Update the playback controls to match the selected sound
		SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

		ui->horizontalSlider_MasterVolume->setValue(sound_metadata->master_volume);
		ui->horizontalSlider_LeftVolume->setValue(sound_metadata->left_volume);
		ui->horizontalSlider_RightVolume->setValue(sound_metadata->right_volume);
		ui->horizontalSlider_Speed->setValue(sound_metadata->speed);
		ui->horizontalSlider_LowPassFilter->setValue(sound_metadata->low_pass_filter_sample_rate);
	}
}

void MainWindow::on_pushButton_Pause_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_SoundPause(sound_metadata->id);
}


void MainWindow::on_pushButton_Unpause_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_SoundUnpause(sound_metadata->id);
}


void MainWindow::on_pushButton_Rewind_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_SoundRewind(sound_metadata->id);
}


void MainWindow::on_pushButton_FadeIn_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_SoundFade(sound_metadata->id, 0x100, 1000 * 5); // Fade over the span of five seconds
}


void MainWindow::on_pushButton_FadeOut_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_SoundFade(sound_metadata->id, 0, 1000 * 5); // Fade over the span of five seconds
}


void MainWindow::on_horizontalSlider_MasterVolume_valueChanged(int value)
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	sound_metadata->master_volume = value;

	UpdateSoundVolume(sound_metadata);
}


void MainWindow::on_horizontalSlider_LeftVolume_valueChanged(int value)
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	sound_metadata->left_volume = value;

	UpdateSoundVolume(sound_metadata);
}


void MainWindow::on_horizontalSlider_RightVolume_valueChanged(int value)
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	sound_metadata->right_volume = value;

	UpdateSoundVolume(sound_metadata);
}


void MainWindow::on_horizontalSlider_Speed_valueChanged(int value)
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	sound_metadata->speed = value;

	ClownAudio_SoundSetSpeed(sound_metadata->id, value);
}


void MainWindow::on_horizontalSlider_LowPassFilter_valueChanged(int value)
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	sound_metadata->low_pass_filter_sample_rate = value;

	ClownAudio_SoundSetLowPassFilter(sound_metadata->id, value);
}

