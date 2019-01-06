namespace CollaborativeTextEditorClient
{
    partial class Form2
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.filesList = new System.Windows.Forms.ListBox();
            this.filesGroupBox = new System.Windows.Forms.GroupBox();
            this.addNewFileButton = new System.Windows.Forms.Button();
            this.newFileTextBox = new System.Windows.Forms.TextBox();
            this.buttonBackToConnSettings = new System.Windows.Forms.Button();
            this.filesGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // filesList
            // 
            this.filesList.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.filesList.FormattingEnabled = true;
            this.filesList.ItemHeight = 18;
            this.filesList.Location = new System.Drawing.Point(29, 36);
            this.filesList.Name = "filesList";
            this.filesList.Size = new System.Drawing.Size(362, 310);
            this.filesList.TabIndex = 0;
            this.filesList.DoubleClick += new System.EventHandler(this.FilesList_DoubleClick);
            // 
            // filesGroupBox
            // 
            this.filesGroupBox.Controls.Add(this.addNewFileButton);
            this.filesGroupBox.Controls.Add(this.newFileTextBox);
            this.filesGroupBox.Controls.Add(this.filesList);
            this.filesGroupBox.Location = new System.Drawing.Point(115, 12);
            this.filesGroupBox.Name = "filesGroupBox";
            this.filesGroupBox.Size = new System.Drawing.Size(415, 426);
            this.filesGroupBox.TabIndex = 1;
            this.filesGroupBox.TabStop = false;
            this.filesGroupBox.Text = "Available Files";
            // 
            // addNewFileButton
            // 
            this.addNewFileButton.Enabled = false;
            this.addNewFileButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.addNewFileButton.Location = new System.Drawing.Point(254, 370);
            this.addNewFileButton.Name = "addNewFileButton";
            this.addNewFileButton.Size = new System.Drawing.Size(137, 33);
            this.addNewFileButton.TabIndex = 4;
            this.addNewFileButton.Text = "Add new";
            this.addNewFileButton.UseVisualStyleBackColor = true;
            this.addNewFileButton.Click += new System.EventHandler(this.AddNewFileButton_Click);
            // 
            // newFileTextBox
            // 
            this.newFileTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.newFileTextBox.Location = new System.Drawing.Point(29, 374);
            this.newFileTextBox.Name = "newFileTextBox";
            this.newFileTextBox.Size = new System.Drawing.Size(219, 24);
            this.newFileTextBox.TabIndex = 3;
            this.newFileTextBox.TextChanged += new System.EventHandler(this.NewFileTextBox_TextChanged);
            // 
            // buttonBackToConnSettings
            // 
            this.buttonBackToConnSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.buttonBackToConnSettings.Location = new System.Drawing.Point(23, 12);
            this.buttonBackToConnSettings.Name = "buttonBackToConnSettings";
            this.buttonBackToConnSettings.Size = new System.Drawing.Size(75, 34);
            this.buttonBackToConnSettings.TabIndex = 2;
            this.buttonBackToConnSettings.Text = "Back";
            this.buttonBackToConnSettings.UseVisualStyleBackColor = true;
            this.buttonBackToConnSettings.Click += new System.EventHandler(this.ButtonBackToConnSettings_Click);
            // 
            // Form2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(554, 450);
            this.Controls.Add(this.buttonBackToConnSettings);
            this.Controls.Add(this.filesGroupBox);
            this.Name = "Form2";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "ListaPlikow";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form2_FormClosing);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form2_FormClosed);
            this.filesGroupBox.ResumeLayout(false);
            this.filesGroupBox.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListBox filesList;
        private System.Windows.Forms.GroupBox filesGroupBox;
        private System.Windows.Forms.Button buttonBackToConnSettings;
        private System.Windows.Forms.TextBox newFileTextBox;
        private System.Windows.Forms.Button addNewFileButton;
    }
}