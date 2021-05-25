#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <cstdarg>
#include <cstdio>
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

	this->setFixedSize(QSize(730, 325));

	ClownAudio_Init();
}

MainWindow::~MainWindow()
{
	// Destroy sounds
	for(int i = 0; i < ui->listWidget_Sounds->count(); ++i)
	{
		QListWidgetItem *item = ui->listWidget_Sounds->item(i);
		SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

		ClownAudio_DestroySound(sound_metadata->id);
		delete sound_metadata;
	}

	// Unload sound data
	for(int i = 0; i < ui->listWidget_SoundData->count(); ++i)
	{
		QListWidgetItem *item = ui->listWidget_SoundData->item(i);
		ClownAudio_SoundData *sound_data = static_cast<ClownAudio_SoundData*>(item->data(Qt::UserRole).value<void*>());

		ClownAudio_UnloadSoundData(sound_data);
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
	ClownAudio_SetSoundVolume(sound_metadata->id, final_left_volume, final_right_volume);
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
	ClownAudio_InitSoundDataConfig(&config);
	config.predecode = ui->checkBox_Predecode->isChecked();
	config.must_predecode = ui->checkBox_MustPredecode->isChecked();
	config.dynamic_sample_rate = ui->checkBox_DataDynamicSampleRate->isChecked();

	// Create sound data
	ClownAudio_SoundData *sound_data = ClownAudio_LoadSoundDataFromFiles(ui->lineEdit_File1->text().toStdString().c_str(), ui->lineEdit_File2->text().toStdString().c_str(), &config);

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

	ClownAudio_UnloadSoundData(sound_data);

	delete item;
}

void MainWindow::on_pushButton_CreateSound_clicked()
{
	QListWidgetItem *item = ui->listWidget_SoundData->currentItem();

	// Set up sound configuration
	ClownAudio_SoundConfig config;
	ClownAudio_InitSoundConfig(&config);
	config.loop = ui->checkBox_Loop->isChecked();
	config.do_not_free_when_done = ui->checkBox_DoNotFreeWhenDone->isChecked();
	config.dynamic_sample_rate = ui->checkBox_InstanceDynamicSampleRate->isChecked();

	ClownAudio_SoundData *sound_data = static_cast<ClownAudio_SoundData*>(item->data(Qt::UserRole).value<void*>());

	ClownAudio_SoundID sound_id = ClownAudio_CreateSound(sound_data, &config);

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

	ClownAudio_DestroySound(sound_metadata->id);

	delete sound_metadata;
	delete item;
}

void MainWindow::on_listWidget_SoundData_itemSelectionChanged()
{
	bool sound_data_selected = ui->listWidget_SoundData->currentItem() != nullptr;

	// Enable or disable parts of the interface
	ui->pushButton_UnloadSoundData->setEnabled(sound_data_selected);

	ui->checkBox_Loop->setEnabled(sound_data_selected);
	ui->checkBox_DoNotFreeWhenDone->setEnabled(sound_data_selected);
	ui->checkBox_InstanceDynamicSampleRate->setEnabled(sound_data_selected);
	ui->pushButton_CreateSound->setEnabled(sound_data_selected);
}

void MainWindow::on_listWidget_Sounds_itemSelectionChanged()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();

	bool sound_instance_selected = item != nullptr;

	// Enable or disable parts of the interface
	ui->pushButton_DestroySound->setEnabled(sound_instance_selected);

	ui->pushButton_Pause->setEnabled(sound_instance_selected);
	ui->pushButton_Unpause->setEnabled(sound_instance_selected);
	ui->pushButton_Rewind->setEnabled(sound_instance_selected);

	ui->pushButton_FadeIn->setEnabled(sound_instance_selected);
	ui->pushButton_FadeOut->setEnabled(sound_instance_selected);
	ui->pushButton_CancelFade->setEnabled(sound_instance_selected);

	ui->label_MasterVolume->setEnabled(sound_instance_selected);
	ui->horizontalSlider_MasterVolume->setEnabled(sound_instance_selected);
	ui->label_LeftVolume->setEnabled(sound_instance_selected);
	ui->horizontalSlider_LeftVolume->setEnabled(sound_instance_selected);
	ui->label_RightVolume->setEnabled(sound_instance_selected);
	ui->horizontalSlider_RightVolume->setEnabled(sound_instance_selected);

	ui->lineEdit_SampleRate->setEnabled(sound_instance_selected);
	ui->pushButton_SetSampleRate->setEnabled(sound_instance_selected);

	if (item != nullptr)
	{
		// Update the playback controls to match the selected sound
		SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

		ui->horizontalSlider_MasterVolume->setValue(sound_metadata->master_volume);
		ui->horizontalSlider_LeftVolume->setValue(sound_metadata->left_volume);
		ui->horizontalSlider_RightVolume->setValue(sound_metadata->right_volume);
	}
}

void MainWindow::on_pushButton_Pause_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_PauseSound(sound_metadata->id);
}


void MainWindow::on_pushButton_Unpause_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_UnpauseSound(sound_metadata->id);
}


void MainWindow::on_pushButton_Rewind_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_RewindSound(sound_metadata->id);
}


void MainWindow::on_pushButton_FadeIn_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_FadeInSound(sound_metadata->id, 1000 * 5); // Fade over the span of five seconds
}


void MainWindow::on_pushButton_FadeOut_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_FadeOutSound(sound_metadata->id, 1000 * 5); // Fade over the span of five seconds
}


void MainWindow::on_pushButton_CancelFade_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	SoundMetadata *sound_metadata = item->data(Qt::UserRole).value<SoundMetadata*>();

	ClownAudio_CancelFade(sound_metadata->id);
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


void MainWindow::on_pushButton_SetSampleRate_clicked()
{
	QListWidgetItem *item = ui->listWidget_Sounds->currentItem();
	ClownAudio_SoundID sound_id = item->data(Qt::UserRole).value<SoundMetadata*>()->id;

	try
	{
		unsigned long sample_rate = std::stoul(ui->lineEdit_SampleRate->text().toStdString(), nullptr, 0);

		ClownAudio_SetSoundSampleRate (sound_id, sample_rate, sample_rate);
	}
	catch (const std::invalid_argument &e)
	{
		Q_UNUSED(e);
	}
	catch (const std::out_of_range &e)
	{
		Q_UNUSED(e);
	}
}
