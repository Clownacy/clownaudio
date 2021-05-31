#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <clownaudio/clownaudio.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_pushButton_File1Open_clicked();

	void on_pushButton_File2Open_clicked();

	void on_pushButton_LoadSoundData_clicked();

	void on_pushButton_UnloadSoundData_clicked();

	void on_pushButton_CreateSound_clicked();

	void on_listWidget_SoundData_itemSelectionChanged();

	void on_listWidget_Sounds_itemSelectionChanged();

	void on_pushButton_DestroySound_clicked();

	void on_pushButton_Pause_clicked();

	void on_pushButton_Unpause_clicked();

	void on_pushButton_Rewind_clicked();

	void on_pushButton_FadeIn_clicked();

	void on_pushButton_FadeOut_clicked();

	void on_horizontalSlider_MasterVolume_valueChanged(int value);

	void on_horizontalSlider_LeftVolume_valueChanged(int value);

	void on_horizontalSlider_RightVolume_valueChanged(int value);

	void on_horizontalSlider_Speed_valueChanged(int value);

private:
	struct SoundMetadata
	{
		ClownAudio_SoundID id;
		ClownAudio_SoundData *data;
		unsigned short master_volume;
		unsigned short left_volume;
		unsigned short right_volume;
		unsigned long speed;
		bool paused;
	};

	Ui::MainWindow *ui;

	static void UpdateSoundVolume(SoundMetadata *sound_metadata);
};
#endif // MAINWINDOW_H
