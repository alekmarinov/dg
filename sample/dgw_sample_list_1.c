/*
	dg_sample_list_1.c
	Test widget library - List
*/

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <dg/dgw_form.h>
#include <dg/dgw_list.h>
#include <dg/dgw_label.h>
#include <dg/dgw_style.h>
#include <dg/dgw_image_list.h>
#include <dg/dg_keys.h>

dgw_form_p main_form;

//const char* IMAGE_PATH="";
const char* IMAGE_PATH="C:\\cvs\\lrun-art\\projects\\aviq\\a1\\image\\menu\\";
#define IMAGE_COUNT 18
const char* test_images[IMAGE_COUNT]=
{
	"albums.png",
	"by_channel.png",
	"cd.png",
	"dvd.png",
	"favorites.png",
	"fm_radio.png",
	"in_advance.png",
	"juke.png",
	"live_tv.png",
	"man_programming.png",
	"movies.png",
	"music.png",
	"rec_schedule.png",
	"rec_schedule_direct.png",
	"search.png",
	"settings.png",
	"whats_on.png",
	"www_radio.png",
};

const int   label_count = 20;
const char* label_captions_0[] = 
{
	"List item 0-0",
	"List item 0-1",
	"List item 0-2",
	"List item 0-3",
	"List item 0-4",
	"List item 0-5",
	"List item 0-6",
	"List item 0-7",
	"List item 0-8",
	"List item 0-9",
	"List item 0-10",
	"List item 0-11",
	"List item 0-12",
	"List item 0-13",
	"List item 0-14",
	"List item 0-15",
	"List item 0-16",
	"List item 0-17",
	"List item 0-18",
	"List item 0-19"
};
const char* label_captions_1[] = 
{
	"List item 1-0",
	"List item 1-1",
	"List item 1-2",
	"List item 1-3",
	"List item 1-4",
	"List item 1-5",
	"List item 1-6",
	"List item 1-7",
	"List item 1-8",
	"List item 1-9",
	"List item 1-10",
	"List item 1-11",
	"List item 1-12",
	"List item 1-13",
	"List item 1-14",
	"List item 1-15",
	"List item 1-16",
	"List item 1-17",
	"List item 1-18",
	"List item 1-19"
};

dgw_form_p create_dialog(dg_widget_p parent, const char* caption, int x, int y, int w, int h, int is_owner)
{
	dgw_form_p dialog=dgw_form_create(parent?parent:DGWidget(main_form), x, y, w, h, caption);
	DGWidgetLayoutAnchorProps(DGWidget(dialog), DG_LAYOUT_ANCHOR_ALL);
	DGWidgetWindow(dialog)->owner_window=is_owner;
	DGWidgetRender(dialog);
	DGWidgetInvalidate(dialog, 0);
	return dialog;
}

int max_tab_index = 1;
dgw_list_p create_list(dg_widget_p parent, dgw_image_list_p image_list, int x, int y, int w, int h, int img_width, int row_height, int is_focus)
{
	int i;
	dgw_list_column_t column;
	dgw_list_p list = dgw_list_create(parent, x, y, w, h);
	DGWidgetLayoutAnchorProps(list, DG_LAYOUT_ANCHOR_ALL);
	DGWidget(list)->tab_index = max_tab_index++;
	column.column_type = LIST_COLUMN_IMAGE;
	column.visible = 1;
	column.width = img_width;
	list->add_column(list, column);
	column.width = w - img_width - 5;
	column.column_type = LIST_COLUMN_TEXT;
	list->add_column(list, column);

	list->row_height = row_height;

	for (i=0; i<label_count; i++)
	{
		dgw_list_row_t row;
		row.cells = malloc(2*sizeof(dgw_list_cell_t));
		row.cells[0].image = i % IMAGE_COUNT;
		row.cells[1].text = (char*)label_captions_0[i];
		row.is_selected = 0;
		row.is_separator = 0;
		list->add_row(list, row);
		free(row.cells);
	}

	list->set_image_list(list, image_list);

	if (is_focus)
		DGWidgetSetFocus(list);

	DGWidgetRender(list);
	DGWidgetInvalidate(list, 0);
	return list;
}

#define DIALOG_WIDTH 700
#define LIST_COUNT 2
#define LIST_SPACE 10

int main()	
{
	dg_video_p video;
	dgw_list_p list[LIST_COUNT];
	dgw_form_p dialog;
	dgw_image_list_p image_list = dgw_image_list_create();
	int i;
	int list_width = ( (DIALOG_WIDTH - LIST_COUNT * LIST_SPACE) / LIST_COUNT);
	int img_height = 0;
	int img_width  = 0;
	main_form=dgw_form_create(0, 0, 0, 800, 600, "Sample Window");
	dialog = create_dialog(DGWidget(main_form), "Sample Dialog", 50, 50, DIALOG_WIDTH, 350, 0);
	DGWidgetLayoutAnchor(main_form);
	DGWidgetLayoutAnchor(dialog);
	
	video = DGWidgetDevice(main_form);

	for (i=0; i<IMAGE_COUNT; i++)
	{
		char image_path[256];
		dg_surface_p image;
		strcpy(image_path, IMAGE_PATH);
		strcat(image_path, test_images[i]);
		image = video->load_surface_file(video, image_path);
		if (image)
		{
			image_list->add(image_list, image);
			if (img_width < image->width) img_width = image->width;
			if (img_height < image->height) img_height = image->height;
		}
		else
			printf("Unable to load image %s\n", image_path);
	}
	
	for (i=0; i<LIST_COUNT; i++)
	{
		list[i]=create_list(DGWidget(dialog), image_list, 5 + i * (list_width + LIST_SPACE), 50, list_width, 240, img_width, img_height, i == 0);
	
		if (i == 0)
			DGWidgetLayoutAnchorProps(list[i], DG_LAYOUT_ANCHOR_ALL);
		else
			DGWidgetLayoutAnchorProps(list[i], DG_LAYOUT_ANCHOR_TOP_BOTTOM_RIGHT);
	}

	DGWidgetFlip(main_form, 0);
	
	DGWidgetEventLoop(main_form);
	image_list->destroy(image_list);
	DGWidgetDestroy(main_form);
	return 0;
}
