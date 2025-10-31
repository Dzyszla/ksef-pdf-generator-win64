// eslint-disable-next-line @typescript-eslint/ban-ts-comment
// @ts-expect-error
import { generateInvoice, generatePDFUPO } from './build/ksef-pdf-generator.es.js';

const input = document.getElementById('xmlInput') as HTMLInputElement;
const inputUPO = document.getElementById('xmlInputUPO') as HTMLInputElement;

input.addEventListener('change', async () => {
  const file = input.files?.[0];

  if (!file) {
    return;
  }
  const additionalData: any = {
    nrKSeF: '5265877635-20250808-9231003CA67B-BE',
    qrCode:
      'https://ksef-te.mf.gov.pl/client-app/invoice/5265877635/26-10-2025/HS5E1zrA8WVjDNq_xMVIN5SD6nyRymmQ-BcYHReUAa0',
  }; // Example data

  generateInvoice(file, additionalData, 'blob').then((data: any) => {
    const url = URL.createObjectURL(data);

    const a = document.createElement('a');

    a.href = url;
    a.download = 'test.pdf'; // nazwa pobieranego pliku
    document.body.appendChild(a);
    a.click();

    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  });
});

inputUPO.addEventListener('change', async () => {
  const file = inputUPO.files?.[0];

  if (!file) {
    return;
  }
  generatePDFUPO(file).then((blob: any) => {
    const url = URL.createObjectURL(blob);

    const a = document.createElement('a');

    a.href = url;
    a.download = 'test.pdf'; // nazwa pobieranego pliku
    document.body.appendChild(a);
    a.click();

    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  });
});
