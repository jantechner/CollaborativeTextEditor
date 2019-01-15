using System;
using System.Text;
using System.Windows.Forms;

namespace CollaborativeTextEditorClient
{
    public partial class Form3 : Form
    {
        private Form1 form1 = null;
        private Form2 form2;
        private Form3 form;
        public Connection conn = null;
        private int fileIndex;

        private int DiffersAtIndex(string s1, string s2)
        {
            int index = 0;
            int min = Math.Min(s1.Length, s2.Length);
            while (index < min && s1[index] == s2[index])
                index++;

            return (index == min && s1.Length == s2.Length) ? -1 : index;
        }

        public delegate void UpdateFileContentCallback(String fileContent);
        public UpdateFileContentCallback UpdateFileContentDelegate;
        public void UpdateFileContent(String fileContent)
        {
            if (this.textBox.InvokeRequired) this.form.Invoke(new UpdateFileContentCallback(UpdateFileContent), fileContent);
            else
            {
                int oldTextLenght = this.textBox.Text.Length, newTextLength = fileContent.Length;

                int cursorPosition = textBox.SelectionStart;

                fileContent = fileContent.Replace("\n", Environment.NewLine);
                int changeIndex = DiffersAtIndex(this.textBox.Text.ToString(), fileContent);

                this.textBox.Text = fileContent;

                if (changeIndex < cursorPosition)
                {
                    if (oldTextLenght < newTextLength) textBox.SelectionStart = cursorPosition - 1;
                    else textBox.SelectionStart = cursorPosition + 1;
                }
                    
            }
        }

        public Form3(String fileContent, Form2 form2, Connection conn, int index)
        {
            InitializeComponent();
            this.form = this;
            this.form2 = form2;
            this.conn = conn;
            this.fileIndex = index;
            UpdateFileContentDelegate = new UpdateFileContentCallback(UpdateFileContent);
            this.textBox.Text = fileContent.Replace("\n", Environment.NewLine);


        }

        private void BackButton_Click(object sender, EventArgs e)
        {
            conn.Send("c#" + fileIndex.ToString());
            this.form.Hide();
            form2.Show();
        }

        private void TextBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            int line = textBox.GetLineFromCharIndex(textBox.SelectionStart);
            int position = textBox.SelectionStart - textBox.GetFirstCharIndexOfCurrentLine();

            if ((int)e.KeyChar == 8)
            {
                Console.WriteLine("Backspace");
                conn.Send("d#" + line + "#" + (position - 1));
            }
            else if ((int)e.KeyChar != 13)
            {
                Console.WriteLine("Normal char");
                conn.Send("i#" + line + "#" + position + "#" + (int)e.KeyChar);
            }
        }

        private void textBox_KeyDown(object sender, KeyEventArgs e)
        {
            int line = textBox.GetLineFromCharIndex(textBox.SelectionStart);
            int position = textBox.SelectionStart - textBox.GetFirstCharIndexOfCurrentLine();

            if (e.KeyCode == Keys.Delete)
            {
                Console.WriteLine(line + " " + position + " " + e.KeyData);
                conn.Send("d#" + line + "#" + position);
            }
            else if (e.KeyCode == Keys.Enter)
            {
                Console.WriteLine(line + " " + position + " Enter");
                conn.Send("n#" + line + "#" + position);

            }
        }

        private void Form3_FormClosed(object sender, FormClosedEventArgs e)
        {
            FormManager.CloseAllForms();
        }

        private void Form3_FormClosing(object sender, FormClosingEventArgs e)
        {
            conn.Close();
        }
    }
}
