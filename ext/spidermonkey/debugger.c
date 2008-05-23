#include "debugger.h"
#include "context.h"

static JSTrapStatus interrupt_handler(JSContext *UNUSED(js), JSScript *UNUSED(script),
                                      jsbytecode *UNUSED(pc), jsval *UNUSED(rval), void *rb)
{
  VALUE self = (VALUE)rb;
  /* FIXME: Pass this stuff to the debugger. */
  rb_funcall(self, rb_intern("interrupt_handler"), 0);
  return JSTRAP_CONTINUE;
}

static void new_script_hook(JSContext *UNUSED(js),
                            const char *UNUSED(filename),
                            uintN UNUSED(lineno),
                            JSScript *UNUSED(script),
                            JSFunction *UNUSED(fun),
                            void *rb)
{
  VALUE self = (VALUE)rb;

  /* FIXME: Pass this crap to the debugger */
  rb_funcall(self, rb_intern("new_script_hook"), 0);
}

static void destroy_script_hook(JSContext *UNUSED(js),
                                JSScript *UNUSED(script),
                                void *rb)
{
  VALUE self = (VALUE)rb;
  rb_funcall(self, rb_intern("destroy_script_hook"), 0);
}

static JSTrapStatus debugger_handler(JSContext *UNUSED(js), JSScript *UNUSED(script),
                                     jsbytecode *UNUSED(pc), jsval *UNUSED(rval), void *rb)
{
  VALUE self = (VALUE)rb;
  /* FIXME: Pass this crap to the debugger? */
  rb_funcall(self, rb_intern("debugger_handler"), 0);
  return JSTRAP_CONTINUE;
}

static void source_handler(const char *filename, uintN lineno,
                           jschar *UNUSED(str), size_t UNUSED(length),
                           void **UNUSED(listenerTSData), void *rb)
{
  VALUE self = (VALUE)rb;
  VALUE rb_filename = rb_str_new2(filename);
  VALUE rb_lineno   = INT2NUM((int)lineno);

  rb_funcall(self, rb_intern("source_handler"), 2, rb_filename, rb_lineno);
}

static void * execute_hook(JSContext *UNUSED(js), JSStackFrame *UNUSED(fp), JSBool before,
                           JSBool *UNUSED(ok), void *rb)
{
  VALUE self = (VALUE)rb;
  VALUE rb_before = JS_TRUE == before ? Qtrue : Qfalse;

  /* FIXME: Should OK be passed? */
  rb_funcall(self, rb_intern("execute_hook"), 1, rb_before);
  return rb;
}

static void * call_hook(JSContext *UNUSED(js), JSStackFrame *UNUSED(fp), JSBool before,
                           JSBool *UNUSED(ok), void *rb)
{
  VALUE self = (VALUE)rb;
  VALUE rb_before = JS_TRUE == before ? Qtrue : Qfalse;

  /* FIXME: Should OK be passed? */
  rb_funcall(self, rb_intern("call_hook"), 1, rb_before);
  return rb;
}

static void object_hook(JSContext *UNUSED(js), JSObject *UNUSED(obj), JSBool UNUSED(isNew), void *rb)
{
  VALUE self = (VALUE)rb;

  /* FIXME: Pass this stuff to the debugger. */
  rb_funcall(self, rb_intern("object_hook"), 0);
}

static JSTrapStatus throw_hook(JSContext *UNUSED(js), JSScript *UNUSED(script),
                               jsbytecode *UNUSED(pc), jsval *UNUSED(rval), void *rb)
{
  VALUE self = (VALUE)rb;
  /* FIXME: Pass this stuff to the debugger? */
  rb_funcall(self, rb_intern("throw_hook"), 0);
  return JSTRAP_CONTINUE;
}

static JSBool debug_error_hook(JSContext *UNUSED(js), const char *message,
                                JSErrorReport *UNUSED(report), void *rb)
{
  VALUE self = (VALUE)rb;
  VALUE rb_message = rb_str_new2(message);
  rb_funcall(self, rb_intern("debug_error_hook"), 1, rb_message);
  return JS_TRUE;
}

static VALUE allocate(VALUE klass)
{
  JSDebugHooks* debug = calloc(1, sizeof(JSDebugHooks));
  VALUE self = Data_Wrap_Struct(klass, 0, 0, debug);

  debug->interruptHandler = interrupt_handler;
  debug->interruptHandlerData = (void *)self;
  debug->newScriptHook = new_script_hook;
  debug->newScriptHookData = (void *)self;
  debug->destroyScriptHook = destroy_script_hook;
  debug->destroyScriptHookData = (void *)self;
  debug->debuggerHandler = debugger_handler;
  debug->debuggerHandlerData = (void *)self;
  debug->sourceHandler = source_handler;
  debug->sourceHandlerData = (void *)self;
  debug->executeHook = execute_hook;
  debug->executeHookData = (void *)self;
  debug->callHook = call_hook;
  debug->callHookData = (void *)self;
  debug->objectHook = object_hook;
  debug->objectHookData = (void *)self;
  debug->throwHook = throw_hook;
  debug->throwHookData = (void *)self;
  debug->debugErrorHook = debug_error_hook;
  debug->debugErrorHookData = (void *)self;

  return self;
}

void init_Johnson_SpiderMonkey_Debugger(VALUE spidermonkey)
{
  /* HACK:  These comments are *only* to make RDoc happy.
  VALUE johnson = rb_define_module("Johnson");
  VALUE spidermonkey = rb_define_module_under(johnson, "SpiderMonkey");
  */

  /* This is the debugging hooks used with SpiderMonkey. */
  VALUE context = rb_define_class_under(spidermonkey, "Debugger", rb_cObject);

  rb_define_alloc_func(context, allocate);
}