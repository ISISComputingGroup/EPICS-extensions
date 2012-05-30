using System;
using System.Windows;
using System.Runtime.InteropServices;
using System.Threading;
using Als.Epics.SimpleChannelAccess;
using Als.Epics.ChannelAccess;

namespace EpicsWpfExample
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Sca _sca = new Sca();
        bool _firstTime = true;
        String _name = "";
        private delegate void UpdateGuiDelegate(String n, String v);

        public MainWindow()
        {
            InitializeComponent();

            //Set up the path
            string path = System.Environment.GetEnvironmentVariable("PATH");

            if (Environment.Is64BitProcess)
            {
                throw new Exception("Cannot run as 64bit at the moment");
            }
            else
            {
                path += ";" + Properties.Settings.Default.EPICS_BASE + "\\win32-x86\\";
            }
            System.Environment.SetEnvironmentVariable("PATH", path);
        }

        private void btnGet_Click(object sender, RoutedEventArgs e)
        {
            if (_firstTime)
            {
                _firstTime = false;
                txtName.IsEnabled = false;
                _name = txtName.Text;
                _sca.AddItem(_name);
            }
            _sca.RefreshValues();
            double ans = _sca.GetDouble(_name);
            txtValue.Text = ans.ToString();
        }

        private void btnSet_Click(object sender, RoutedEventArgs e)
        {
            if (_firstTime)
            {
                _firstTime = false;
                txtName.IsEnabled = false;
                _name = txtName.Text;
                _sca.AddItem(_name);
            }

            double res;

            if (Double.TryParse(txtNewValue.Text, out res))
            {
                _sca.PutDouble(_name, res);
            }
            else
            {
                MessageBox.Show("Please enter a valid double");
            }

        }

        private void btnSetEvent_Click(object sender, RoutedEventArgs e)
        {
            if (_firstTime)
            {
                _firstTime = false;
                txtName.IsEnabled = false;
                _name = txtName.Text;
                _sca.AddItem(_name);
            }

            btnSetEvent.IsEnabled = false;

            Thread mon = new Thread(monitor);
            mon.Start();
        }

        private void monitor()
        {
            Sca sca = new Sca();
            sca.AddItem(_name);

            Ca.EventCallbackDelagte callback = new Ca.EventCallbackDelagte(pv_ValueChanged);
            Boolean res = sca.Subscribe(_name, callback);
            Ca.ca_pend_event(0.0);
        }

        unsafe void pv_ValueChanged(Ca.event_handler_args args)
        {
            if (args.chid != IntPtr.Zero)
            {
                String name = Marshal.PtrToStringAnsi(new IntPtr((void*)Ca.ca_name(args.chid)));
                Object val = (object)*(double*)args.dbr;

                this.Dispatcher.Invoke(new UpdateGuiDelegate(UpdateEvent), name, val.ToString());
            }
        }

        void UpdateEvent(String name, String val)
        {
            txtEventInfo.Text = name + ": " + val.ToString();
        }
    }
}