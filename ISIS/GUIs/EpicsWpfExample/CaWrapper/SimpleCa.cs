using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Security;

namespace CaWrapper
{
    public class PV
    {
        public String name { get; private set; }
        public int chid { get; private set; }
        public short chtype { get; private set; }
        public uint elementCount { get; private set; }

        public PV(String pvname, int chanid, short type, uint elemCount)
        {
            name = pvname;
            chid = chanid;
            chtype = type;
            elementCount = elemCount;
        }
    }

    public class SimpleCa
    {
        Dictionary<String, PV> _pvs = new Dictionary<string, PV>();

        public void CreateChannel(String name)
        {
            if (!_pvs.Keys.Contains(name.ToLower()))
            {
                IntPtr chanID = IntPtr.Zero;
                ErrorCode res = (ErrorCode)Ca.ca_create_channel(name, null, IntPtr.Zero, 0, ref chanID);

                if (res != ErrorCode.ECA_NORMAL)
                {
                    throw new Exception("Could not create channel. Error code is " + res.ToString());
                }

                res = (ErrorCode)Ca.ca_pend_io(2.0);

                if (res != ErrorCode.ECA_NORMAL)
                {
                    throw new Exception("Could not create channel. Error code is " + res.ToString());
                }

                short type = Ca.ca_field_type(chanID);

                uint elem = Ca.ca_element_count(chanID);

                _pvs[name.ToLower()] = new PV(name, chanID.ToInt32(), type, elem);
            }
        }

        /// <summary>
        /// 
        /// Note: This "halts" all further action on the current thread.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="callback"></param>
        public void CreateSubscription(String name, Ca.EventCallBackDelegate callback)
        {
            if (_pvs.Keys.Contains(name.ToLower()))
            {
                PV temp = _pvs[name.ToLower()];
                IntPtr chanID = new IntPtr(temp.chid);
                IntPtr evtId = IntPtr.Zero;
                ErrorCode res = (ErrorCode)Ca.ca_create_subscription(temp.chtype, temp.elementCount, chanID, (int)SubscriptionMask.DBE_VALUE, callback, IntPtr.Zero, ref evtId);

                if (res != ErrorCode.ECA_NORMAL)
                {
                    throw new Exception("Could not subscribe to channel. Error code is " + res.ToString());
                }
            }
            else
            {
                throw new Exception("Could not subscribe to channel as channel does not exist. Try calling \"CreateChannel\"");
            }
        }

        public void EnableAllSubscriptions(double timeout)
        {
            int res = Ca.ca_pend_event(timeout);
        }

        private IntPtr getRawValue(PV pv)
        {
            IntPtr chanID = new IntPtr(pv.chid);

            IntPtr value = IntPtr.Zero;

            if ((ChannelType)pv.chtype == ChannelType.DBR_STRING)
            {
                //The max length of a string is 80 bytes (unconfirmed!)
                value = Marshal.AllocHGlobal((int)pv.elementCount * 80);
            }
            else
            {
                //Doubles, ints etc are 8 bytes
                value = Marshal.AllocHGlobal((int)pv.elementCount * 8);
            }

            ErrorCode res = (ErrorCode)Ca.ca_array_get(pv.chtype, pv.elementCount, chanID, value);

            if (res != ErrorCode.ECA_NORMAL)
            {
                throw new Exception("Could not get value. Error code is " + res.ToString());
            }

            res = (ErrorCode)Ca.ca_pend_io(0.0);

            if (res != ErrorCode.ECA_NORMAL)
            {
                throw new Exception("Could not get value. Error code is " + res.ToString());
            }

            return value;
        }

        public double GetDouble(String name)
        {
            if (_pvs.Keys.Contains(name.ToLower()))
            {
                PV temp = _pvs[name.ToLower()];

                if ((ChannelType)temp.chtype != ChannelType.DBR_DOUBLE)
                {
                    throw new Exception("Could not get value. As channel type is not a double");
                }

                IntPtr raw = IntPtr.Zero;

                try
                {
                    raw = getRawValue(temp);
                    double val = CastToDouble(raw);
                    return val;
                }
                catch (Exception err)
                {
                    throw err;
                }
                finally
                {
                    Marshal.FreeHGlobal(raw);
                }
            }
            else
            {
                throw new Exception("Could not get value as channel does not exist. Try calling \"CreateChannel\"");
            }
        }

        public int GetInteger(String name)
        {
            if (_pvs.Keys.Contains(name.ToLower()))
            {
                PV temp = _pvs[name.ToLower()];

                if ((ChannelType)temp.chtype != ChannelType.DBR_INT)
                {
                    throw new Exception("Could not get value. As channel type is not an integer");
                }

                IntPtr raw = IntPtr.Zero;

                try
                {
                    raw = getRawValue(temp);
                    int val = CastToInt(raw);
                    return val;
                }
                catch (Exception err)
                {
                    throw err;
                }
                finally
                {
                    Marshal.FreeHGlobal(raw);
                }
            }
            else
            {
                throw new Exception("Could not get value as channel does not exist. Try calling \"CreateChannel\"");
            }
        }

        public unsafe static double CastToDouble(IntPtr value)
        {
            return (double)(object)*(double*)value.ToPointer();
        }

        public unsafe static int CastToInt(IntPtr value)
        {
            return (int)(object)*(int*)value.ToPointer();
        }

        private unsafe IntPtr createDoublePointer(uint count, double value)
        {
            IntPtr ptrVal = IntPtr.Zero;
            ptrVal = Marshal.AllocHGlobal((int)count * 8);
            *(double*)ptrVal.ToPointer() = value;
            return ptrVal;
        }

        public void PutDouble(String name, double value)
        {
            if (_pvs.Keys.Contains(name.ToLower()))
            {
                PV temp = _pvs[name.ToLower()];

                if ((ChannelType)temp.chtype != ChannelType.DBR_DOUBLE)
                {
                    throw new Exception("Could not get value. As channel type is not a double");
                }

                IntPtr ptrVal = IntPtr.Zero;

                try
                {
                    ptrVal = createDoublePointer(temp.elementCount, value);

                    ErrorCode res = (ErrorCode)Ca.ca_array_put(temp.chtype, temp.elementCount, new IntPtr(temp.chid), ptrVal);

                    if (res != ErrorCode.ECA_NORMAL)
                    {
                        throw new Exception("Could not set value. Error code is " + res.ToString());
                    }

                    res = (ErrorCode)Ca.ca_pend_io(0.0);

                    if (res != ErrorCode.ECA_NORMAL)
                    {
                        throw new Exception("Could not set value. Error code is " + res.ToString());
                    }
                }
                catch (Exception err)
                {
                    throw err;
                }
                finally
                {
                    Marshal.FreeHGlobal(ptrVal);
                }
            }
            else
            {
                throw new Exception("Could not get value as channel does not exist. Try calling \"CreateChannel\"");
            }
        }
    }
}
