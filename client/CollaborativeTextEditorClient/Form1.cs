using System;
using System.Windows.Forms;

namespace CollaborativeTextEditorClient
{
    public partial class Form1 : Form
    {
        public Form1 form;
        Connection conn = null;

        public delegate void SetStatusLabelCallback(String text);
        public SetStatusLabelCallback SetStatusLabelDelegate;
        public void SetStatusLabel(String text)
        {
            if (this.statusStrip.InvokeRequired) this.form.Invoke(new SetStatusLabelCallback(SetStatusLabel), text);
            else this.toolStripStatusLabel1.Text = text;
        }

        public delegate void SetButtonCallback(bool status);
        public SetButtonCallback SetButtonDelegate;
        public void SetButton(bool status)
        {
            if (this.buttonConnect.InvokeRequired) this.form.Invoke(new SetButtonCallback(SetButton), status);
            else this.buttonConnect.Enabled = status;
        }

        public delegate void OpenNextFormCallback(String filesString, Form2 form2);
        public OpenNextFormCallback OpenNextFormDelegate;
        public void OpenNextForm(String filesString, Form2 form2)
        {
            if (this.form.InvokeRequired) this.form.Invoke(new OpenNextFormCallback(OpenNextForm), filesString, form2);
            else
            {
                this.form.Hide();
                form2.Show();
            }
        }

        public Form1()
        {
            InitializeComponent();
            this.form = this;
            SetStatusLabelDelegate = new SetStatusLabelCallback(SetStatusLabel);
            SetButtonDelegate = new SetButtonCallback(SetButton);
            OpenNextFormDelegate = new OpenNextFormCallback(OpenNextForm);
        }

        private void ButtonConnectClicked(object sender, EventArgs e)
        {
            try
            {
                String formAddress = this.textBoxAddr.Text;
                String formPort = this.textBoxPort.Text;
                SetButton(false);

                if (formAddress.Length > 0 && formPort.Length > 0)
                {
                    if (conn != null) conn.Close();
                    conn = new Connection(form, formAddress, formPort);
                    conn.CreateSocket();
                    conn.Connect();
                }
                else
                {
                    if (formAddress.Length <= 0) MessageBox.Show("No server address!");
                    else if (formPort.Length <= 0) MessageBox.Show("No server port number!");
                    SetButton(true);
                    SetStatusLabel("Check \"Server Info\" and try again!");
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show("11. Exception:\t\n" + exc.Message.ToString());
                SetStatusLabel("Check \"Server Info\" and try again!");
                SetButton(true);
            }
        }

        private void Form1Closing(object sender, FormClosingEventArgs e)
        {
            try
            {
                if (conn != null) conn.Close();
            }
            catch (Exception exc)
            {
                MessageBox.Show("12. Exception:\t\n" + exc.Message.ToString());
            }
        }

        private void Form1Closed(object sender, FormClosedEventArgs e)
        {
            FormManager.CloseAllForms();
        }
    }


    public class FormManager
    {
        public static void CloseAllForms()
        {
            foreach (Form f in Application.OpenForms)
            {
                f.Close();
            }
            Environment.Exit(0);
        }
    }

}