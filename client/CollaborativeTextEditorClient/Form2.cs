using System;
using System.Windows.Forms;


namespace CollaborativeTextEditorClient
{
    public partial class Form2 : Form
    {
        private Form1 form1;
        private Form2 form;
        private Form3 form3;
        public Connection conn = null;

        public delegate void UpdateFilesListCallback(String filesString);
        public UpdateFilesListCallback UpdateFilesListDelegate;
        public void UpdateFilesList(String filesString)
        {
            if (this.filesList.InvokeRequired) this.form.Invoke(new UpdateFilesListCallback(UpdateFilesList), filesString);
            else
            {
                this.filesList.Items.Clear();
                foreach (String s in filesString.Split('#'))
                {
                    this.filesList.Items.Add(s);
                }
                this.newFileTextBox.Text = "";
            }
        }

        public delegate void OpenNextFormCallback(String fileContent, Form3 form3);
        public OpenNextFormCallback OpenNextFormDelegate;
        public void OpenNextForm(String fileContent, Form3 form3)
        {
            if (this.form.InvokeRequired) this.form.Invoke(new OpenNextFormCallback(OpenNextForm), fileContent, form3);
            else
            {
                this.form.Hide();
                form3.Show();
            }
        }


        public Form2(String filesString, Form1 form1, Connection conn)
        {
            InitializeComponent();
            this.form = this;
            this.form1 = form1;
            this.conn = conn;
            Console.WriteLine(filesString);
            UpdateFilesListDelegate = new UpdateFilesListCallback(UpdateFilesList);
            OpenNextFormDelegate = new OpenNextFormCallback(OpenNextForm);

            this.filesList.Items.Clear();
            foreach (String s in filesString.Split('#'))
            {
                this.filesList.Items.Add(s);
            }
            this.filesList.SetSelected(0, true);
        }

        private void ButtonBackToConnSettings_Click(object sender, EventArgs e)
        {
            this.form.Hide();
            form1.Show();
        }

        private void FilesList_DoubleClick(object sender, EventArgs e)
        {
            try
            {
                String message = "o#" + this.filesList.SelectedItem.ToString();
                Console.WriteLine(message);
                conn.Send(message);
            }
            catch (Exception exc)
            {
                MessageBox.Show("21. Exception:\t\n" + exc.Message.ToString());
            }
        }

        private void NewFileTextBox_TextChanged(object sender, EventArgs e)
        {
            if (newFileTextBox.Text.Length != 0) addNewFileButton.Enabled = true;
            else addNewFileButton.Enabled = false;
        }

        private void AddNewFileButton_Click(object sender, EventArgs e)
        {
            try
            {
                String message = "a#" + this.newFileTextBox.Text;
                Console.WriteLine(message);
                this.addNewFileButton.Enabled = false;
                conn.Send(message);
            }
            catch (Exception exc)
            {
                MessageBox.Show("22. Exception:\t\n" + exc.Message.ToString());
            }
        }

        private void Form2_FormClosed(object sender, FormClosedEventArgs e)
        {
            FormManager.CloseAllForms();
        }

        private void Form2_FormClosing(object sender, FormClosingEventArgs e)
        {
            conn.Close();
        }
    }
}
