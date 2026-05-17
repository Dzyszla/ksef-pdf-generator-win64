unit KSeFPDFGenerator.Exe;
	
interface

type
	TXMLType = (xtFA, xtUPO);
	TEnvironment = (evTest, evDemo, evProd);
	
	TGenOptions = record
		Watermark: String;
		LangFileName: String;
	end;		

procedure GenPDF(const aXML: TStream; ANrKSeF: string; const aXMLType: TXMLType; const aEnvironment: TEnvironment; const aOutStream: TStream; const aOptions: TGenOptions);

implementation

uses
	System.Classes,
	System.SysUtils,
	WinAPI.Windows;

const
	C_QR1: array[Low(TEnvironment)..High(TEnvironment)] of string =
		('https://qr-test.ksef.mf.gov.pl/invoice/{nip}/{p1}/{hash}',
		'https://qr-demo.ksef.mf.gov.pl/invoice/{nip}/{p1}/{hash}',
		'https://qr.ksef.mf.gov.pl/invoice/{nip}/{p1}/{hash}'
		);

procedure GenPDF(const aXML: TStream; ANrKSeF: string; const aXMLType: TXMLType; const aEnvironment: TEnvironment; const aOutStream: TStream; const aOptions: TGenOptions);
const
	C_BUFF_SIZE = 32 * 1024;
var
	SI: TStartupInfo;
	PI: TProcessInformation;
	Sec: TSecurityAttributes;
	StdInRead, StdInWrite: THandle;
	StdOutRead, StdOutWrite: THandle;
	Cmd: string;
	BytesRead, BytesWritten: Cardinal;
	Buffer: array [0 .. C_BUFF_SIZE - 1] of Byte;
	res: Integer;
	tmpStream: TStringStream;
begin
	// Rurki muszą być dziedziczone
	Sec.nLength := SizeOf(Sec);
	Sec.lpSecurityDescriptor := nil;
	Sec.bInheritHandle := True;

	// STDIN pipe
	if not CreatePipe(StdInRead, StdInWrite, @Sec, 0) then
		RaiseLastOSError;
	if not SetHandleInformation(StdInWrite, HANDLE_FLAG_INHERIT, 0) then
		RaiseLastOSError;

	// STDOUT pipe
	if not CreatePipe(StdOutRead, StdOutWrite, @Sec, 0) then
		RaiseLastOSError;
	if not SetHandleInformation(StdOutRead, HANDLE_FLAG_INHERIT, 0) then
		RaiseLastOSError;

	ZeroMemory(@SI, SizeOf(SI));
	SI.cb := SizeOf(SI);
	SI.dwFlags := STARTF_USESTDHANDLES or STARTF_USESHOWWINDOW;
	SI.wShowWindow := SW_HIDE;
	SI.hStdInput := StdInRead;
	SI.hStdOutput := StdOutWrite;
	SI.hStdError := StdOutWrite; // też przekierowujemy na wypadek błędów

	ZeroMemory(@PI, SizeOf(PI));

	if aXMLType = xtUPO then
		Cmd := Format('"%sKSeF-PDFGen.exe" --stream -t upo', [ExtractFilePath(ParamStr(0))])
	else if aXMLType.IsFa then
	begin
		if aNrKSeF.IsEmpty then
			aNrKSeF := '/NIE NADANY/';
		Cmd := Format('"%sKSeF-PDFGen.exe" --stream --nrKSeF "%s" --qrCode "%s" -t invoice', [ExtractFilePath(ParamStr(0)), ANrKSeF, C_QR1[aEnvironment]]);
	end
	else
		raise Exception.Create('Unknown XML type');

	if not aOptions.Watermark.IsEmpty then
		Cmd := Format('%s --watermark "%s"', [Cmd, aWatermark.Replace('"', '""')]);

	if not aOptions.LangFileName.IsEmpty and FileExists(aOptions.LangFileName) then
		Cmd := Format('%s --lang %s', [Cmd, QuotedStr(aOptions.LangFileName)]);

	if not CreateProcess(nil, PChar(Cmd), nil, nil, True { dziedziczenie uchwytów},	CREATE_NO_WINDOW, nil, nil, SI, PI) then
		RaiseLastOSError;
	CloseHandle(StdInRead);   StdInRead := 0;
	CloseHandle(StdOutWrite); StdOutWrite := 0;

	// ---- Piszemy dane do STDIN procesu ----
	try
		if Assigned(aXML) then
		begin
			aXML.Position := 0;
			while True do
			begin
				BytesRead := aXML.Read(Buffer, SizeOf(Buffer));
				if BytesRead = 0 then
					Break;

				if not WriteFile(StdInWrite, Buffer, BytesRead, BytesWritten, nil) then
					RaiseLastOSError;

				if BytesRead <> BytesWritten then
					raise Exception.Create('Proces pipe write error');
			end;
		end;
		// Zamykamy STDIN – proces musi wiedzieć, że nie będzie więcej danych
		CloseHandle(StdInWrite); StdInWrite := 0;

		// ---- Czytamy STDOUT procesu ----
		if Assigned(aOutStream) then
		begin
			repeat
				if not ReadFile(StdOutRead, Buffer, SizeOf(Buffer), BytesRead, nil) then
					Break;
				if BytesRead > 0 then
					aOutStream.WriteBuffer(Buffer, BytesRead);
			until BytesRead = 0;
		end;
		aOutStream.Position := 0;

		// Czekamy na zakończenie procesu
		WaitForSingleObject(PI.hProcess, INFINITE);
		GetExitCodeProcess(PI.hProcess, DWORD(res));
		if res <> 0 then
		begin
			tmpStream := TStringStream.Create('', TEncoding.UTF8);
			try
				tmpStream.LoadFromStream(aOutStream);
				aOutStream.Size := 0;
				raise Exception.CreateFmt('PDF Generator Run-time exception #%d: %s', [res, tmpStream.DataString]);
			finally
				tmpStream.Free;
			end;
		end;
	finally
		if StdInWrite <> 0 then
			CloseHandle(StdInWrite);
		if StdOutRead <> 0 then
			CloseHandle(StdOutRead);

		CloseHandle(PI.hThread);
		CloseHandle(PI.hProcess);
	end;
end;
