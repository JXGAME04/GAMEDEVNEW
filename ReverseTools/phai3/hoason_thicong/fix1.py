# sua hs_data1.py: Linux skills_table.lua la CRLF -> chuan hoa truoc regex; ten tep ranksetting VLTK
import io, os, glob
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "hs_data1.py")
s = io.open(p, "r", encoding="utf-8").read()
old = '    lin = rd(os.path.join(LIN, "script", "global", "skills_table.lua"))\n'
new = '    lin = rd(os.path.join(LIN, "script", "global", "skills_table.lua")).replace("\\r\\n", "\\n")\n'
assert old in s
s = s.replace(old, new)
io.open(p, "w", encoding="utf-8").write(s)
print("ok; vltk_raw ranksetting:", glob.glob(r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw\*rank*"))
