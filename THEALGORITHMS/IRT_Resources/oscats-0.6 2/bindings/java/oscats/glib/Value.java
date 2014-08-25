/*
 * Value.java
 *
 * Copyright (c) 2006-2009 Operational Dynamics Consulting Pty Ltd
 * 
 * The code in this file, and the library it is a part of, are made available
 * to you by the authors under the terms of the "GNU General Public Licence,
 * version 2" plus the "Classpath Exception" (you may link to this code as a
 * library into other programs provided you don't make a derivation of it).
 * See the LICENCE file for the terms governing usage and redistribution.
 */
package oscats.glib;

import oscats.bindings.Constant;
import oscats.bindings.Debug;
import oscats.bindings.Pointer;
import java.lang.reflect.Array;

/**
 * A generic value that can be passed as a parameter to or returned from a
 * method or function on an underlying entity in the GLib library and those
 * built on it. Value is only used in setting or getting Object properties,
 * and in the TreeModel system.
 * 
 * <p>
 * <b>For use by bindings hackers only.</b><br>
 * As with other classes in <code>org.gnome.glib</code>, this is
 * implementation machinery and should not be needed by anyone developing
 * applications with java-gnome.
 * 
 * <p>
 * Ironically, Values are <i>not</i> actually type safe; if you happen to
 * create one to hold Strings, and then call the getEnum() method on it, your
 * program will explode (this is somewhat to the contrary of the spirit of
 * java-gnome). These are therefore only for use from within strongly typed
 * methods exposing a safe and sane public API. You've been warned.
 * 
 * <p>
 * <i>Complementing the object oriented system supplied by the GLib library is
 * a set of foundation elements, <code>GType</code> and <code>GValue</code>,
 * the latter being defined as "a polymorphic type that can hold values of any
 * other type", which isn't much help, really.</i>
 * 
 * <p>
 * <i>Since instances of Java classes are their own identity, we do not need
 * to directly represent <code>GType</code> and <code>GValue</code> as
 * separate classes. We implement <code>GType</code> as a characteristic that
 * any</i> <code>Value</code> <i>or</i> <code>Object</code> <i>has.</i>
 * 
 * @author Andrew Cowie
 * @since 4.0.0
 */
public class Value extends Pointer
{
    /*
     * The second argument is a hack to create a different overload of
     * <init>(long). What a pain in the ass, but whatever.
     */
    protected Value(long pointer, boolean proxy) {
        super(pointer);
        if (Debug.MEMORY_MANAGEMENT) {
            System.err.println("Value.<init>(long)\t\t" + this.toString());
            System.err.flush();
        }
    }

    /**
     * By design, we are the owner of all the GValues because we allocate the
     * memory for them in (for example) the native implementation of
     * {@link GValue#createValue(int)}. So, call <code>g_slice_free()</code>
     * on our pointer to free that memory.
     */
    protected void release() {
        if (Debug.MEMORY_MANAGEMENT) {
            System.err.println("Value.release()\t\t\t" + this.toString());
        }
        GValue.free(this);
    }

    /**
     * Create an empty Value without initializing it's type. For use in
     * methods like TreeModel's
     * {@link org.gnome.gtk.TreeModel#getValue(org.gnome.gtk.TreeIter, org.gnome.gtk.DataColumnString)
     * getValue()} family, which use a blank Value internally.
     * 
     * Not public API!
     */
    protected Value() {
        this(GValue.createValue(), true);
    }

    /**
     * Dispatch based on generic java.lang.Object
     */
    protected static Value fromObject(java.lang.Object value) {
      if (value.getClass().isArray())
      {
        int num = Array.getLength(value);
        Value[] vals = new Value[num];
        for (int i=0; i < num; i++)
          vals[i] = fromObject(Array.get(value, i));
        return new Value(GValue.createValue(vals), true);
      }
      if (value instanceof String)
        return new Value(GValue.createValue((String)value), true);
      else if (value instanceof Object)		// glib.Object
        return new Value(GValue.createValue((Object)value), true);
      else if (value instanceof Constant)
        return new Value(GValue.createValue((Constant)value), true);
      else if (value instanceof Integer)
        return new Value(GValue.createValue(((Integer)value).intValue()), true);
      else if (value instanceof Long)
        return new Value(GValue.createValue(((Long)value).longValue()), true);
      else if (value instanceof Boolean)
        return new Value(GValue.createValue(((Boolean)value).booleanValue()), true);
      else if (value instanceof Float)
        return new Value(GValue.createValue(((Float)value).floatValue()), true);
      else if (value instanceof Double)
        return new Value(GValue.createValue(((Double)value).doubleValue()), true);
      else if (value instanceof Character)
        return new Value(GValue.createValue(((Character)value).charValue()), true);
      else
        throw new ClassCastException("Cannot create Value for " + value.getClass().getName());
    }

    /**
     * Create a Value containing a String. Not public API!
     */
    protected Value(String value) {
        this(GValue.createValue(value), true);
    }

    protected String getString() {
        return GValue.getString(this);
    }

    /**
     * Create a Value containing an <code>int</code>. Not public API!
     */
    protected Value(int value) {
        this(GValue.createValue(value), true);
    }

    protected int getInteger() {
        return GValue.getInteger(this);
    }

    /**
     * Create a Value containing a <code>boolean</code>. Not public API!
     */
    protected Value(boolean value) {
        this(GValue.createValue(value), true);
    }

    protected boolean getBoolean() {
        return GValue.getBoolean(this);
    }

    /**
     * Create a Value containing a <code>float</code>. Not public API!
     */
    protected Value(float value) {
        this(GValue.createValue(value), true);
    }

    protected Value(double value) {
        this(GValue.createValue(value), true);
    }

    protected Value(Object obj) {
        this(GValue.createValue(obj), true);
    }

    /*
     * Another one that's only really here for unit tests.
     */
    protected Object getObject() {
        return GValue.getObject(this);
    }

    protected float getFloat() {
        return GValue.getFloat(this);
    }

    protected double getDouble() {
        return GValue.getDouble(this);
    }

    protected Value(long value) {
        this(GValue.createValue(value), true);
    }

    protected Value(Constant value) {
        this(GValue.createValue(value), true);
    }

    protected long getLong() {
        return GValue.getLong(this);
    }

    protected Constant getEnum() {
        return GValue.getEnum(this);
    }
}
