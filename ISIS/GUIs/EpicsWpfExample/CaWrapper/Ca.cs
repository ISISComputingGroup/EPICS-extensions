using System;
using System.Runtime.InteropServices;
using System.Security;

namespace CaWrapper
{
    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct connection_handler_args
    {
        // chanId->chid->oldChannelNotify*
        IntPtr chid;
        //int
        int op;
    }

    [StructLayoutAttribute(LayoutKind.Sequential)]
    public struct event_handler_args
    {
        // void*
        public IntPtr usr;
        // chanId->chid->oldChannelNotify*
        public IntPtr chid;
        // int
        public int type;
        // int
        public int count;
        // void*
        public IntPtr dbr;
        // int
        public int status;
    }

    public class Ca
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        [SuppressUnmanagedCodeSecurity]
        public delegate void ConnStateCallbackDelgate(connection_handler_args args);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        [SuppressUnmanagedCodeSecurity]
        public delegate void EventCallBackDelegate(event_handler_args args);

        [SuppressUnmanagedCodeSecurity]
        [STAThread]
        [DllImport("ca.dll")]
        public extern static int ca_create_channel([MarshalAs(UnmanagedType.LPStr)] string chanName, ConnStateCallbackDelgate connStateCallback, IntPtr userPrivate, uint priority, ref IntPtr chanID);

        [SuppressUnmanagedCodeSecurity]
        [STAThread]
        [DllImport("ca.dll")]
        public static extern int ca_clear_channel(IntPtr chanId);

        [SuppressUnmanagedCodeSecurity]
        [DllImport("ca.dll")]
        public extern static int ca_array_get(int type, uint count, IntPtr chanID, IntPtr value);

        [SuppressUnmanagedCodeSecurity]
        [DllImport("ca.dll")]
        public static extern int ca_array_put(int type, uint count, IntPtr chanId, IntPtr pValue);

        [SuppressUnmanagedCodeSecurity]
        [DllImport("ca.dll")]
        public static extern int ca_pend_io(double timeOut);

        [SuppressUnmanagedCodeSecurity]
        [DllImport("ca.dll")]
        public static extern uint ca_element_count(IntPtr chan);

        [SuppressUnmanagedCodeSecurity]
        [DllImport("ca.dll")]
        public static extern short ca_field_type(System.IntPtr chan);

        [SuppressUnmanagedCodeSecurity]
        [STAThread]
        [DllImport("ca.dll")]
        public static extern int ca_create_subscription(int type, uint count, IntPtr chanID, int mask, EventCallBackDelegate eventCallBackDelegate, IntPtr arg, ref IntPtr eventID);

        [SuppressUnmanagedCodeSecurity]
        [STAThread]
        [DllImport("ca.dll")]
        public static extern int ca_clear_subscription(IntPtr eventID);

        [SuppressUnmanagedCodeSecurity]
        [DllImport("ca.dll")]
        public static extern ChannelState ca_state(IntPtr chan);

        [SuppressUnmanagedCodeSecurity]
        [DllImport("ca.dll")]
        public static extern int ca_add_masked_array_event(int type, uint count, IntPtr chanId, EventCallBackDelegate pFunc, IntPtr pArg, double p_delta, double n_delta, double timeout, ref IntPtr pEventID, int mask);

        [SuppressUnmanagedCodeSecurity]
        [STAThread]
        [DllImport("ca.dll")]
        public static extern int ca_pend_event(double timeOut);

        [SuppressUnmanagedCodeSecurity]
        [STAThread]
        [DllImport("ca.dll")]
        public static extern int ca_clear_event(System.IntPtr eventID);

        [SuppressUnmanagedCodeSecurity]
        [DllImport("ca.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr ca_name(IntPtr chan);

        [SuppressUnmanagedCodeSecurity]
        [STAThread]
        [DllImport("ca.dll")]
        public static extern void ca_context_destroy();

        [SuppressUnmanagedCodeSecurity]
        [STAThread]
        [DllImport("ca.dll")]
        public static extern void ca_detach_context();

        [SuppressUnmanagedCodeSecurity]
        [DllImport("ca.dll")]
        public static extern uint dbr_size_n(int type, uint count);

    }
}
