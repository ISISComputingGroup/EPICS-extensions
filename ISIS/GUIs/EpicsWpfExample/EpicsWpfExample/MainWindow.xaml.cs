using System;
using System.Windows;
using System.Runtime.InteropServices;
using System.Threading;

namespace EpicsWpfExample
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        CaWrapper.SimpleCa _sca = new CaWrapper.SimpleCa();
        bool _firstTime = true;
        String _name = "";
        private delegate void UpdateGuiDelegate(String n, String v);
        bool _stop = false;

        public MainWindow()
        {
            InitializeComponent();

            //Set up the path
            string path = System.Environment.GetEnvironmentVariable("PATH");

            if (Environment.Is64BitProcess)
            {
                path += ";" + Properties.Settings.Default.EPICS_BASE + "\\windows-x64\\";
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
                _sca.CreateChannel(_name);
            }

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
                _sca.CreateChannel(_name);
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
                _sca.CreateChannel(_name);
            }

            btnSetEvent.IsEnabled = false;

            Thread mon = new Thread(monitor);
            mon.Start();
        }

        private void monitor()
        {
            CaWrapper.SimpleCa sca = new CaWrapper.SimpleCa();
            sca.CreateChannel(_name);

            CaWrapper.Ca.EventCallBackDelegate callback = new CaWrapper.Ca.EventCallBackDelegate(pv_ValueChanged);
            sca.CreateSubscription(_name, callback);
            sca.EnableAllSubscriptions(0.0);
        }

        unsafe void pv_ValueChanged(CaWrapper.event_handler_args args)
        {
            if (args.chid != IntPtr.Zero)
            {
                String name = Marshal.PtrToStringAnsi(CaWrapper.Ca.ca_name(args.chid));
                Double val = CaWrapper.SimpleCa.CastToDouble(args.dbr);

                this.Dispatcher.Invoke(new UpdateGuiDelegate(UpdateEvent), name, val.ToString());
            }
        }

        void UpdateEvent(String name, String val)
        {
            txtEventInfo.Text = name + ": " + val.ToString();
        }

        private void Window_Closed(object sender, EventArgs e)
        {

        }
    }
}