/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_event.h                                         */
/* Description:   Event Management interface                         */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_EVENT_H
#define __DG_EVENT_H

#include <dg/dg.h>

/*!
	\addtogroup dg_events Events

	This section describes the event driven aspect of DirectGUI.
	Two roles in event correspondence are considered. The first role is related to the
	event triggerers while the second to the event listeners.
	dg_event_manager_t is the general object connected to with the event correspondence.
	It represents an event box where listeners can register themselves to listen for specific
	event types. See EVT_TYPE_* defines for the different event types supported by DirectGUI.

	@{
*/

/*! Quit event type */
#define EVT_TYPE_QUIT               (1<<0)

/*! Keyboard event type  */
#define EVT_TYPE_KEYBOARD           (1<<1)

/*! Mouse event type  */
#define EVT_TYPE_MOUSE              (1<<2)

/*! Timer event type  */
#define EVT_TYPE_TIMER              (1<<3)

/*! Animation event type  */
#define EVT_TYPE_ANIMATION          (1<<4)

/*! Widget event type  */
#define EVT_TYPE_WIDGET             (1<<5)

/*! Scrollbar event type  */
#define EVT_TYPE_SCROLLBAR          (1<<6)


/* Mouse events */

/*! Left mouse button pressed or released */
#define EVT_MOUSE_LBUTTON           (1<<0)

/*! Right mouse button pressed or released */
#define EVT_MOUSE_RBUTTON           (1<<1)

/*! Middle mouse button pressed or released */
#define EVT_MOUSE_MBUTTON           (1<<2)

/*! Mouse wheel spined */
#define EVT_MOUSE_WHEEL             (1<<3)

/*! Mouse pressing is doubled */
#define EVT_MOUSE_DBL               (1<<4)

/*! The button is pressed */
#define EVT_PRESSED                 (1<<5)

/*! The button is released */
#define EVT_RELEASED                (1<<6)

/* Widget events */

/*! Widget is moved */
#define EVT_WIDGET_MOVE             (1<<0)

/*! Widget is resized */
#define EVT_WIDGET_SIZE             (1<<1)

/*! Mouse moved over the widget */
#define EVT_WIDGET_MOUSE_MOVE       (1<<2)

/*! Mouse wheel spined over the widget */
#define EVT_WIDGET_MOUSE_WHEEL      (1<<3)

/*! Mouse dragging over the widget */
#define EVT_WIDGET_MOUSE_DRAGGING   (1<<4)

/*! Mouse start dragging the widget */
#define EVT_WIDGET_MOUSE_DRAG_START (1<<5)

/*! Mouse stop dragging the widget */
#define EVT_WIDGET_MOUSE_DRAG_STOP  (1<<6)

/*! Mouse enter in widget region */
#define EVT_WIDGET_MOUSE_ENTER      (1<<7)

/*! Mouse leave the widget region */
#define EVT_WIDGET_MOUSE_LEAVE      (1<<8)

/*! Widget focus status changed */
#define EVT_WIDGET_FOCUS            (1<<9)

/*! Widget status is changed (e.g. button pressed or released) */
#define EVT_WIDGET_STATUS_CHANGED   (1<<10)

/*! Widget surface rendered */
#define EVT_WIDGET_RENDER           (1<<11)

/*! 
* \brief Event structure holding the data connected with an event
*/
typedef struct _dg_event_t
{
	/* ! Which event. A mask of EVT_TYPE_* */
	int type;                  

	/* ! Specific event information */
	union
	{
		/* ! Keyboard event information */
		struct
		{
			/*! Which keyboard event */
			int key_event;     

			/*! Which key if the event is from a keyboard */
			int key;           

			/*! ALT, SHIFT, CTRL status mask */
			int modifiers;     
		} keyboard;

		/* ! Mouse event information */
		struct
		{
			/*! Which mouse event */
			int mouse_event;   

			/*! Mouse x-axis */
			int x;            

			/*! Mouse y-axis */
			int y;     
		} mouse;

		/* ! Widget event information */
		struct
		{
			/*! Which widget event */
			int widget_event;  

			/*! Widget event param 1 */
			unsigned long p1;  

			/*! Widget event param 2 */
			unsigned long p2;  
		} widget;

		/* ! Custom event information */
		struct
		{
			/*! Custom param */
			int n0;
			
			/*! Custom param */
			int n1;
			
			/*! Custom param */
			int n2;
			
			/*! Custom param */
			int n3;
		} custom;
	} e;
} dg_event_t, *dg_event_p;

/*! 
* \brief ScrollBar event structure connecting a scrollbar with a scrollable widget
*/
typedef struct _dg_event_scrollbar_t
{
	/*! Common event structure */
	dg_event_t e;

	/*! Amount of item to be scrolled */
	int item_count;
	
	/*! Amount of the visible items */
	int visible_item_count;

	/*! Scroll offset of the first visible item in all items */
	int scroll;
} dg_event_scrollbar_t, *dg_event_scrollbar_p;

/*! 
* \brief Button statuses
*/
typedef enum
{
	BUTTON_STATUS_PRESSED,
	BUTTON_STATUS_RELEASED,
} dg_button_status_t;

/*! 
* \brief Button event structure
*/
typedef struct _dg_event_t dg_event_button_t, *dg_event_button_p;

/*! 
* \brief Event listener callback. It must return 1 if the listener handles the event.
*/
typedef int (*dg_event_listener_t)(
	/*! Custom parameter */
	void*, 

	/*! Triggered event */
	dg_event_p
	);

/*! 
* \brief Linked list of listeners
*/
typedef struct _dg_event_listener_list_t
{
	/*! 
	* Listener event mask. Describes which general events the listener can handle.
	* See EVT_TYPE_* defines  
	*/
	int event_type_mask;

	/*! Listener callback function */
	dg_event_listener_t listener;
	
	/*! Animation duration */
	unsigned int animation_duration;

	/*! Animation begin time */
	unsigned int animation_begin;

	/*! Animation ticker */
	float animation_ticker;

	/*! Custom parameter to be provided as first parameter to the callback function */
	void* param;

	/*! The next listener in the list */
	struct _dg_event_listener_list_t* next;

	/*! The previous listener in the list */
	struct _dg_event_listener_list_t* prev;
} dg_event_listener_list_t, *dg_event_listener_list_p;

/*! 
* \brief Event Manager is an event box where listeners can be registered for specific events
*/
typedef struct _dg_event_manager_t
{
	/*! 
	* True if this event manager is user for master events
	*/
	int master;

	/*! 
	* If event loop is started to this event box
	* this flag set the loop to stop 
	*/
	int active;

	/*! Linked list of listeners */
	dg_event_listener_list_p listeners;

	/*! Read and remove event from queue */
	int  (*peek)(struct _dg_event_manager_t*, dg_event_p event);

	/*! Executes one peek/trigger loop */
	void (*idle)(struct _dg_event_manager_t*);
	
	/*! Repeats idle method until active is true */
	void (*loop)(struct _dg_event_manager_t*);

	/*! Add new listener */
	void (*add_listener)(struct _dg_event_manager_t*, 
		/*! Listener callback function */
		dg_event_listener_t, 
		/*! Event mask the listener is interested from */
		int, 
		/*! 
		* Custom parameter to be provided as a 
		* first parameter to the listener callback 
		*/
		void*
		);

	/*! Add new animation listener */
	void (*add_animation_listener)(struct _dg_event_manager_t*, 
		/*! Listener callback function */
		dg_event_listener_t, 
		/*! 
		* Custom parameter to be provided as a 
		* first parameter to the listener callback 
		*/
		void*,
		/*!
		* Animation duration in milliseconds (1/1000 secs)
		*/
		unsigned int
		);

	/*! Trigger an event to this event box */
	int  (*trigger_event)(struct _dg_event_manager_t*, 
		/*! The triggered event */
		dg_event_p
		);

	/*! Destroys the event box */
	void (*destroy)(struct _dg_event_manager_t*);
} dg_event_manager_t, *dg_event_manager_p;

struct _dg_widget_t;

/*! 
* \brief Creates new event manager
*/
DG_API dg_event_manager_p dg_event_manager_create( void );

/*! 
* \brief Creates new master event manager (managing events like keyboard and mouse)
*/
DG_API dg_event_manager_p dg_event_manager_master_create();

/*! 
* \brief Set widget to capture mouse events
*/
void dg_event_capture_widget(struct _dg_widget_t*, int);

/*! 
* \brief Notify event manager for widget before its destruction
*/
void dg_event_notify_destroy_widget(struct _dg_widget_t*);

/*! 
* \brief Set the master widget which is the root of the application widgets tree
*/
void dg_event_set_master_widget(struct _dg_widget_t*);

/*! 
* \brief Focus a widget to receive the keyboard events
*/
int dg_event_focus_widget(struct _dg_widget_t*);

/*! 
* \brief Returns true if the widget or any it its children is focused.
*/
int dg_event_is_focus_widget(struct _dg_widget_t*);

/*! 
* \brief Removes the focus from the given widget, bypassing the focus to its parent
*/
void dg_event_kill_focus_widget(struct _dg_widget_t*);

/*! @} */

#endif
