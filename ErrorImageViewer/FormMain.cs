using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace ErrorImageViewer
{
    public partial class FormMain : Form
    {
        public FormMain()
        {
            InitializeComponent();
        }

        private double mErrorValue;
        private DirectoryInfo[] mFolderList;
        private FileInfo[] mSubFileList;
        private int mIndex = -1;
        private int mSubIndex = -1;
        private String mStrResultImagePath = null;
        private String mStrCorrectFolder = null;

        private void FormMain_Load(object sender, EventArgs e)
        {
            MessageBox.Show("直下にテストフォルダがある場所にExeを置いてください");

            // エラー値のディフォルトを設定する
            txtErrorValue.Text = ""+Properties.Settings.Default.ErrorValue;

            GetFolderList();
            NextImage();
        }

        private void GetFolderList()
        {
            var dir = new DirectoryInfo(".");
            mFolderList = dir.GetDirectories("*");
        }

        private void GetSubFolderList()
        {
            String strDirectoy = mFolderList[mIndex].FullName + @"\Result";
            if (!Directory.Exists(strDirectoy))
            {
                Directory.CreateDirectory(strDirectoy);
            }
            var subdir = new DirectoryInfo(strDirectoy);
            mSubFileList = subdir.GetFiles(@"*.png");
            mSubIndex = 0;
        }

        // http://dobon.net/vb/dotnet/graphics/drawpicture2.html
        public static System.Drawing.Image CreateImage(string filename)
        {
            System.IO.FileStream fs = new System.IO.FileStream(
                filename,
                System.IO.FileMode.Open,
                System.IO.FileAccess.Read);
            System.Drawing.Image img = System.Drawing.Image.FromStream(fs);
            fs.Close();
            return img;
        }

        private double CalcErrorValueFromAllFiles(out Bitmap imgFinalDiff, out Bitmap imgResult, out Bitmap imgCorrect)
        {
            // コメントがあれば読む
            String strCommentPath = mFolderList[mIndex].FullName + @"\readme.txt";
            if (File.Exists(strCommentPath))
            {
                using (var rs = new StreamReader(strCommentPath, Encoding.UTF8))
                {
                    AddLog(rs.ReadLine());
                }
            }

            mStrResultImagePath = null;
            imgResult = imgCorrect = imgFinalDiff = new Bitmap(1,1);

            if (mSubIndex == 0)
                GetSubFolderList();

            if (mSubFileList.Length == 0)
                return 0.0;

            // Resultsフォルダ内に対応する画像があるか？
            String strResultImagePath = mSubFileList[mSubIndex].FullName;
            mStrResultImagePath = strResultImagePath;

            String strFileTitle = mSubFileList[mSubIndex].Name;
            strFileTitle = strFileTitle.Substring(0, strFileTitle.Length-mSubFileList[mSubIndex].Extension.Length);
            String strCompFolder = mFolderList[mIndex].FullName + @"\correct\" + strFileTitle;

            mStrCorrectFolder = strCompFolder;

            // 
            if (Directory.Exists(strCompFolder))
            {
                // 比較フォルダがあるとき
                Bitmap imgCur = new Bitmap(CreateImage(strResultImagePath));

                var files = new DirectoryInfo(strCompFolder);
                var imgfiles = files.GetFiles(@"*.png");

                double dMinErrorValue = double.MaxValue;

                // すべてのファイルを比較して，最小のエラー値を求める
                foreach (var fileComp in imgfiles)
                {

                    Bitmap imgComp = new Bitmap(CreateImage(fileComp.FullName));
                    Bitmap imgDiff;
                    double dErrorValue = CalcErrorValue(out imgDiff, imgCur, imgComp);
                    if (dErrorValue < dMinErrorValue)
                    {
                        dMinErrorValue = dErrorValue;
                        imgFinalDiff = imgDiff;
                        imgResult = imgCur;
                        imgCorrect = imgComp;
                    }
                }
                return dMinErrorValue;
            }
            else
            {
                // 比較フォルダがないので初期ファイルを用意する
                AddLog(strCompFolder + "を作成して初期ファイルを置きました");
                Directory.CreateDirectory(strCompFolder);
                File.Move( strResultImagePath, strCompFolder + @"\" + strFileTitle + @".png");

                return 0.0;
            }
        }

        // 一枚の画像のErrorValueを計算する
        private double CalcErrorValue(out Bitmap imgDiff, Bitmap img1, Bitmap img2)
        {
            imgDiff = new Bitmap(1, 1);
            if (img1.Width != img2.Width || img1.Height != img2.Height)
                return double.MaxValue;

            imgDiff = new Bitmap(img1.Width, img1.Height);

            double dErrorValue = 0.0;

            for (int iy = 0; iy < img1.Height; ++iy)
            {
                for (int ix = 0; ix < img1.Width; ++ix)
                {
                    var pixel1 = img1.GetPixel(ix, iy);
                    var pixel2 = img2.GetPixel(ix, iy);
                    var pixel3 = Color.FromArgb(pixel1.R ^ pixel2.R, pixel1.G ^ pixel2.G, pixel1.B ^ pixel2.B);
                    imgDiff.SetPixel(ix, iy, pixel3);

                    dErrorValue += (pixel3.R != 0 || pixel3.G != 0 || pixel3.B != 0) ? 1 : 0;
                }
            }

            return dErrorValue;
        }

        // 次のエラーイメージに移動する
        private void NextImage()
        {
            ++mSubIndex;
            if ( mSubFileList == null || mSubFileList.Length <= mSubIndex ) {
                mSubFileList = null;
                mSubIndex = 0;

                if ( mFolderList.Length-1 > mIndex ) {

                    ++mIndex;
                    GetSubFolderList();
                }
                else {

                    MessageBox.Show("もうデータがありません");
                    return;
                }
            }

            double dPassErrorValue = 0.0;
            double.TryParse( txtErrorValue.Text, out dPassErrorValue );

            Bitmap imgDiff, imgResult, imgCorrect;
            double dErrorValue = CalcErrorValueFromAllFiles(out imgDiff, out imgResult, out imgCorrect );
            if (dErrorValue <= dPassErrorValue)
            {
                AddLog("Pass Error Value : " + dErrorValue);
                NextImage();
            }
            else
            {
                AddLog("Compare " + mStrCorrectFolder + " ErrorValue : " + ("" + dErrorValue));
                pictureBox1.Image = imgResult;
                pictureBox2.Image = imgDiff;
                pictureBox3.Image = imgCorrect;
                txtCurErrorValue.Text = "" + dErrorValue;
            }
        }

        private void AddLog( String strMes ) {
            txtLog.Text = strMes + "\r\n" + txtLog.Text;
        }

        private void txtErrorValue_TextChanged(object sender, EventArgs e)
        {
            double dVal = 0.0;
            double.TryParse(txtErrorValue.Text, out dVal);
            Properties.Settings.Default.ErrorValue = dVal;
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            Properties.Settings.Default.Save();
        }

        private void btnOKImg_Click(object sender, EventArgs e)
        {
            // OK画像のときは，Resultの画像を消して，Correct画像のフォルダにこの画像を追加する
            if (mStrResultImagePath != null)
            {
                String strMoveFileName = (new FileInfo(mStrResultImagePath)).Name;
                int nCnt = 0;
                for (; ; ++nCnt )
                {
                    try
                    {
                        File.Move(mStrResultImagePath, mStrCorrectFolder + @"\" + strMoveFileName + "." + (""+nCnt)+".png");
                        break;
                    }
                    catch (IOException)
                    {

                    }
                }
            }
            NextImage();
        }

        private void btnErrorImg_Click(object sender, EventArgs e)
        {
            // Error画像のときは特に何もせず．
            NextImage();
        }

        private void btnGoBack_Click(object sender, EventArgs e)
        {
            // 最初に戻る
            mIndex = -1;
            mSubIndex = -1;
            NextImage();
        }

        private void btnNewImgSeries_Click(object sender, EventArgs e)
        {
            if (mStrCorrectFolder == null)
                return;

            // 新しい画像シリーズ
            // 既存の画像を削除して，フォルダにコピーする
            Directory.Delete(mStrCorrectFolder, true);
            Directory.CreateDirectory(mStrCorrectFolder);

            String strMoveFileName = (new FileInfo(mStrResultImagePath)).Name;
            File.Move(mStrResultImagePath, mStrCorrectFolder + @"\" + strMoveFileName + "." + (""+0)+".png");

            NextImage();
        }
    }
}
