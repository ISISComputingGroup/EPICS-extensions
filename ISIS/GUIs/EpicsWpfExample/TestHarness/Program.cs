using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Runtime.InteropServices;

namespace TestHarness
{
    class Program
    {
        static IntPtr _chanID;
        //static CaWrapper.Ca.EventCallBackDelegate callback;
        static CaWrapper.Ca ca;
        static IntPtr evtID;
        static Boolean end = false;
        static Boolean ended = false;

        public static void monitor()
        {
            CaWrapper.SimpleCa sca = new CaWrapper.SimpleCa();
            sca.CreateChannel("test:rand");
            CaWrapper.Ca.EventCallBackDelegate callback1 = new CaWrapper.Ca.EventCallBackDelegate(pv_ValueChanged1);
            sca.CreateSubscription("test:rand", callback1);

            sca.CreateChannel("test:randasync");
            CaWrapper.Ca.EventCallBackDelegate callback2 = new CaWrapper.Ca.EventCallBackDelegate(pv_ValueChanged2);
            sca.CreateSubscription("test:rand", callback2);

            //IntPtr chanId = IntPtr.Zero;
            //int chid = CaWrapper.Ca.ca_create_channel("test:rand", null, IntPtr.Zero, 0,ref chanId);
            //int res = CaWrapper.Ca.ca_pend_io(0.0);

            //CaWrapper.Ca.EventCallBackDelegate callback = new CaWrapper.Ca.EventCallBackDelegate(pv_ValueChanged);

            //IntPtr evtID = IntPtr.Zero;
            //CaWrapper.Ca.ca_create_subscription(6, 1U, chanId, (int)CaWrapper.SubscriptionMask.DBE_VALUE, callback, IntPtr.Zero, ref evtID);
            sca.EnableAllSubscriptions(0.0);
        }

        static unsafe void pv_ValueChanged1(CaWrapper.event_handler_args args)
        {
            if (args.chid != IntPtr.Zero)
            {
                String name = Marshal.PtrToStringAnsi(CaWrapper.Ca.ca_name(args.chid));
                Object val = (object)*(double*)args.dbr;

                Console.WriteLine(name + " has changed to " + val.ToString());
            }
        }

        static unsafe void pv_ValueChanged2(CaWrapper.event_handler_args args)
        {
            if (args.chid != IntPtr.Zero)
            {
                String name = Marshal.PtrToStringAnsi(CaWrapper.Ca.ca_name(args.chid));
                Object val = (object)*(double*)args.dbr;

                Console.WriteLine(name + " has changed to " + val.ToString() + " (async)");
            }
        }

        static void Main(string[] args)
        {
            //Set up the path
            string path = System.Environment.GetEnvironmentVariable("PATH");
            path += ";C:\\EPICS\\base\\bin\\win32-x86\\";
            System.Environment.SetEnvironmentVariable("PATH", path);

            CaWrapper.SimpleCa sca = new CaWrapper.SimpleCa();

            sca.CreateChannel("test:rand");

            double res = sca.GetDouble("test:rand");

            sca.PutDouble("test:rand", 5);

            //uint size = CaWrapper.Ca.dbr_size_n(6, 1);

            //CaWrapper.Ca ca = new CaWrapper.Ca();

            //int chid = ca.CreateChannel("test:rand");
            //Console.WriteLine(chid);

            //ca.PendIO(0.0);

            //short type = CaWrapper.Ca.ca_field_type(new IntPtr(chid));

            //double val = ca.GetDouble(chid);

            //Console.WriteLine(val);

            //String name = Marshal.PtrToStringAnsi(CaWrapper.Ca.ca_name(new IntPtr(chid)));

            //Thread monThread = new Thread(new ThreadStart(monitor));
            //monThread.Start();

            //Console.ReadLine();

            //end = true;

            //Console.ReadLine();

            //monThread.Abort();

            Console.ReadLine();

        }
    }
}
