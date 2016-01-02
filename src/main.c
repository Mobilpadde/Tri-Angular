#include <pebble.h>
#include <config.h>

#ifndef JS_READY
#define JS_READY 0
#endif

#ifndef JS_BEAUTY
#define JS_BEAUTY 0
#endif

static Window *window, *windowLayer;
static Layer *dotsLayer, *clockLayer, *timeLayer;

static int hours = 0, minutes = 0, seconds = 0;

static int BEAUTY_COLOR_static;

// Hours to angle
static float getAngleHour(int hour){
	return (hour * 360) / 12;
}

// Minutes to angle, also seconds will be handled here
static float getAngleMinute(int minute){
	return (minute * 360) / 60;
}

static void drawDot(GPoint pos, GContext *ctx, GColor col, int radius){
	graphics_context_set_fill_color(ctx, col);
	graphics_fill_circle(ctx, pos, radius);
}

static void drawDotHandle(GPoint pos, GContext *ctx, GColor col, int radius){
	graphics_context_set_stroke_color(ctx, col);
	graphics_context_set_stroke_width(ctx, TRIANGLE_WIDTH);
	graphics_draw_circle(ctx, pos, radius);

	graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
	graphics_fill_circle(ctx, pos, radius);
}

static void drawTriangle(GContext *ctx, GPath *path){
	graphics_context_set_fill_color(ctx, GColorFromHEX(BEAUTY_COLOR_static));

	graphics_context_set_antialiased(ctx, true);
	graphics_context_set_stroke_color(ctx, GColorFromHEX(BEAUTY_COLOR_static));
	graphics_context_set_stroke_width(ctx, 2);

	gpath_draw_filled(ctx, path);
	gpath_draw_outline(ctx, path);

	gpath_destroy(path);
}

static void drawTime(GContext *ctx){
	graphics_context_set_text_color(ctx, BACKGROUND_COLOR);
	graphics_draw_text(ctx, "00:00", fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49), GRect(0,64,144,118), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void makeDots(Layer *layer, GContext *ctx){
	GRect bounds = layer_get_bounds(layer);
	GRect frame = grect_inset(bounds, GEdgeInsets(INSET));

	/*for(int i = 0; i < 60; i++){
		float angle = getAngleMinute(i);
		GPoint pos = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(angle));
		drawDot(pos, ctx, SMALL_DOTS_COLOR, MINUTE_RADIUS);
	}*/

	for(int i = 0; i < 12; i++){
		float angle = getAngleHour(i);
		GPoint pos = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(angle));
		drawDot(pos, ctx, GColorFromHEX(BEAUTY_COLOR_static), HOUR_RADIUS);
	}
}

static void dotsProc(Layer *layer, GContext *ctx){
	makeDots(layer, ctx);
}

static void clockProc(Layer *layer, GContext *ctx){
	GRect bounds = layer_get_bounds(layer);
	GRect frame = grect_inset(bounds, GEdgeInsets(INSET));

	float angleH = getAngleHour(hours);
	GPoint posH = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(angleH));

	float angleM = getAngleMinute(minutes);
	GPoint posM = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(angleM));

	float angleS = getAngleMinute(seconds);
	GPoint posS = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(angleS));

	GPathInfo pathInfo = {
		3,
		(GPoint []) { posH, posM, posS }
	};
	GPath *path = gpath_create(&pathInfo);
	drawTriangle(ctx, path);

	drawDotHandle(posH, ctx, HOUR_COLOR, HOUR_HANDLE_RADIUS);
	drawDotHandle(posM, ctx, MINUTE_COLOR, MINUTE_HANDLE_RADIUS);
	drawDotHandle(posS, ctx, SECOND_COLOR, SECOND_HANDLE_RADIUS);
	
	//drawTime(ctx);
}

void updateTime(){
	time_t tmp = time(NULL);
	struct tm *tickTime = localtime(&tmp);

	hours = tickTime->tm_hour;
	minutes = tickTime->tm_min;
	seconds = tickTime->tm_sec;

	layer_mark_dirty(clockLayer);
}

void tickHandler(struct tm *tick, TimeUnits unitsChanged){
	updateTime();
}

static void inboxReceivedHandler(DictionaryIterator *iter, void *ctx){
	Tuple *readyTuple = dict_find(iter, JS_READY);
	Tuple *beautyTuple = dict_find(iter, JS_BEAUTY);

	if(readyTuple){
		if(BEAUTY_COLOR != 1 && BEAUTY_COLOR != 0){
			BEAUTY_COLOR_static = BEAUTY_COLOR;
		}else{
			BEAUTY_COLOR_static = TRIANGLE_COLOR;
		}
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Connected to JS");
		persist_write_int(JS_READY, 1);
	}
	
	if(beautyTuple && beautyTuple->value->int32 != 1){
		int beautyColor = beautyTuple->value->int32;

		APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", beautyColor);

		BEAUTY_COLOR_static = beautyColor;

		APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", BEAUTY_COLOR_static);

		layer_mark_dirty(clockLayer);
	}
}

void init(void) {
	//BEAUTY_COLOR_static = persist_read_int(BEAUTY_COLOR);
	//BEAUTY_COLOR_static = TRIANGLE_COLOR; //BEAUTY_COLOR == 1 ? TRIANGLE_COLOR : BEAUTY_COLOR;

	// Create a window and text layer
	window = window_create();

	window_set_background_color(window, BACKGROUND_COLOR);

	// Get window information
	Layer *windowLayer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(windowLayer);

	// Create dots canvas
	dotsLayer = layer_create(bounds);
	layer_set_update_proc(dotsLayer, dotsProc);

	// Create clock canvas
	clockLayer = layer_create(bounds);
	layer_set_update_proc(clockLayer, clockProc);

	// Create time canvas
	timeLayer = layer_create(bounds);
	
	// Add layers to parents
	layer_add_child(windowLayer, clockLayer);
	layer_add_child(windowLayer, dotsLayer);
	layer_add_child(windowLayer, timeLayer);

	// Push the window
	window_stack_push(window, true);

	// Listen for seconds
	updateTime();
	tick_timer_service_subscribe(SECOND_UNIT, tickHandler);

	app_message_register_inbox_received(inboxReceivedHandler);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

	APP_LOG(APP_LOG_LEVEL_DEBUG, "I'm hecka awesome!");
}

void deinit(void) {
	// Set persistent memory
	persist_write_int(BEAUTY_COLOR, BEAUTY_COLOR_static);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", BEAUTY_COLOR);

	// Destroy time
	layer_destroy(timeLayer);
	
	// Destory canvases
	layer_destroy(dotsLayer);
	layer_destroy(clockLayer);

	// Destroy the window
	window_destroy(windowLayer);
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
